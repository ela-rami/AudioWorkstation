#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    // Register supported audio formats
    formatManager.registerBasicFormats();
    
    // Configure the transport source
    transportSource.addChangeListener(this);

    // Start the audio thread
    thread.startThread();
    
    // Initialize the audio system
    setAudioChannels(0, 2); // 0 inputs, 2 outputs (stereo)
}

AudioEngine::~AudioEngine()
{
    // Close the audio system
    shutdownAudio();

    // Stop the audio thread
    transportSource.setSource(nullptr);
    thread.stopThread(500); // 500ms timeout
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Prepare the transport source
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        // No file loaded, generate silence
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    // Get audio from the transport source
    transportSource.getNextAudioBlock(bufferToFill);
}

void AudioEngine::releaseResources()
{
    transportSource.releaseResources();
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            listeners.call(&Listener::playbackStarted);
        else
            listeners.call(&Listener::playbackStopped);
    }
}

bool AudioEngine::loadFile(const juce::File& file)
{
    juce::Logger::writeToLog("AudioEngine: Tentativo di caricamento file: " + file.getFullPathName());

    // Verifica che il file esista
    if (!file.existsAsFile())
    {
        juce::Logger::writeToLog("AudioEngine::loadFile - File non esiste: " + file.getFullPathName());
        return false;
    }
    
    // Interrompi la riproduzione corrente
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    
    // Crea un lettore per il file
    auto* reader = formatManager.createReaderFor(file);
    
    if (reader == nullptr)
    {
        juce::Logger::writeToLog("AudioEngine::loadFile - Impossibile creare reader per: " + file.getFullPathName());
        return false;
    }
    
    // Crea sorgente dal reader
    readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    
    // Imposta il looping per riproduzione continua
    readerSource->setLooping(true);
    
    // Configura la sorgente di trasporto
    transportSource.setSource(readerSource.get(), 
                              32768,                 // dimensione buffer prefetch
                              &thread,               // thread di lettura
                              reader->sampleRate);   // sample rate originale
    
    // Notifica i listener
    juce::MessageManager::callAsync([this, file, reader]() {
        listeners.call(&Listener::fileLoaded, file, reader->sampleRate, reader->numChannels);
    });
    
    juce::Logger::writeToLog("AudioEngine: File caricato con successo: " + file.getFullPathName());
    return true;
}

void AudioEngine::play()
{
    if (readerSource.get() == nullptr)
        return;
    
    transportSource.start();
}

void AudioEngine::stop()
{
    transportSource.stop();
}

float AudioEngine::getPositionRelative() const
{
    if (readerSource.get() == nullptr)
        return 0.0f;
    
    // Calcola la posizione relativa tenendo conto del looping
    auto currentPosition = transportSource.getCurrentPosition();
    auto totalLength = transportSource.getLengthInSeconds();
    
    // Se il file è in loop, manteniamo la posizione relativa tra 0.0 e 1.0
    if (totalLength > 0.0)
        return static_cast<float>(std::fmod(currentPosition, totalLength) / totalLength);
    
    return 0.0f;
}

void AudioEngine::setBPM(int bpm)
{
    if (bpm > 0 && bpm != currentBPM)
    {
        currentBPM = bpm;
        
        // Time stretching will be implemented here
        
        listeners.call(&Listener::bpmChanged, currentBPM);
    }
}

void AudioEngine::setKey(const juce::String& key)
{
    if (key != currentKey)
    {
        currentKey = key;
        
        // Pitch shifting will be implemented here
        
        listeners.call(&Listener::keyChanged, currentKey);
    }
}

void AudioEngine::addListener(Listener* listener)
{
    listeners.add(listener);
}

void AudioEngine::removeListener(Listener* listener)
{
    listeners.remove(listener);
}