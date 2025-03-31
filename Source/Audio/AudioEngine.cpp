#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    formatManager.registerBasicFormats();
    thread.startThread(juce::Thread::Priority::normal);
    setAudioChannels(0, 2);
}

AudioEngine::~AudioEngine()
{
    shutdownAudio();
    mixerSource.removeAllInputs();
    trackSources.clear();
    thread.stopThread(1000);
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    const juce::ScopedLock lock(sourceLock);

    // CORREZIONE DEFINITIVA: Rimosso il check sul numero di sorgenti.
    // Se l'engine non sta suonando, esci. Il mixer gestirà correttamente 0 sorgenti.
    if (!engineIsPlaying)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Chiedi al mixer di riempire il buffer.
    mixerSource.getNextAudioBlock(bufferToFill);
}

void AudioEngine::releaseResources()
{
    mixerSource.releaseResources();
}

bool AudioEngine::loadFile(const juce::File& file, int trackId)
{
    juce::Logger::writeToLog("AudioEngine: Loading file: " + file.getFullPathName() + " for track " + juce::String(trackId));

    if (!file.existsAsFile())
    {
        juce::Logger::writeToLog("AudioEngine Error: File does not exist: " + file.getFullPathName());
        return false;
    }

    auto* reader = formatManager.createReaderFor(file);
    if (reader == nullptr)
    {
        juce::Logger::writeToLog("AudioEngine Error: Cannot create reader for: " + file.getFullPathName());
        return false;
    }

    TrackAudioSource newSource;
    newSource.reader = std::unique_ptr<juce::AudioFormatReader>(reader);
    newSource.readerSource = std::make_unique<juce::AudioFormatReaderSource>(newSource.reader.get(), false);
    newSource.transportSource = std::make_unique<juce::AudioTransportSource>();

    newSource.readerSource->setLooping(true);
    newSource.transportSource->setSource(newSource.readerSource.get(),
                                         32768,
                                         &thread,
                                         reader->sampleRate);

    {
        const juce::ScopedLock lock(sourceLock);
        removeTrackAudio_internal(trackId);
        mixerSource.addInputSource(newSource.transportSource.get(), false);
        trackSources[trackId] = std::move(newSource);

         if (engineIsPlaying)
         {
             if (auto* transport = trackSources[trackId].transportSource.get())
             {
                 transport->start();
             }
         }
    }

    juce::Logger::writeToLog("AudioEngine: File loaded successfully for track " + juce::String(trackId));

    juce::MessageManager::callAsync([this, file, trackId]() {
        listeners.call(&Listener::fileLoaded, file, trackId);
    });

    return true;
}

void AudioEngine::removeTrackAudio_internal(int trackId)
{
     auto it = trackSources.find(trackId);
     if (it != trackSources.end())
     {
         juce::Logger::writeToLog("AudioEngine: Removing audio source for track " + juce::String(trackId) + " (internal)");
         auto* transport = it->second.transportSource.get();
         if (transport)
         {
             mixerSource.removeInputSource(transport);
             transport->stop();
             transport->setSource(nullptr);
         }
          trackSources.erase(it);
          juce::Logger::writeToLog("AudioEngine: Track " + juce::String(trackId) + " removed from map.");
     }
}

void AudioEngine::removeTrackAudio(int trackId)
{
    const juce::ScopedLock lock(sourceLock);
    juce::Logger::writeToLog("AudioEngine: Request to remove audio for track " + juce::String(trackId));
    removeTrackAudio_internal(trackId);
}


void AudioEngine::play()
{
    if (!engineIsPlaying)
    {
        const juce::ScopedLock lock(sourceLock);
        engineIsPlaying = true;
        for (auto const& [id, source] : trackSources)
        {
            if (source.transportSource)
            {
                 source.transportSource->start();
            }
        }
        juce::Logger::writeToLog("AudioEngine: Playback started.");
    }
}

void AudioEngine::stop()
{
    if (engineIsPlaying)
    {
        const juce::ScopedLock lock(sourceLock);
        engineIsPlaying = false;
        for (auto const& [id, source] : trackSources)
        {
            if (source.transportSource)
                source.transportSource->stop();
        }
        juce::Logger::writeToLog("AudioEngine: Playback stopped.");
    }
}

float AudioEngine::getPositionRelative(int trackId) const
{
    auto it = trackSources.find(trackId);
    if (it != trackSources.end() && it->second.transportSource)
    {
        auto* transport = it->second.transportSource.get();
        auto totalLength = transport->getLengthInSeconds();
        if (totalLength > 0.0)
        {
            auto currentPosition = transport->getCurrentPosition();
            return static_cast<float>(std::fmod(currentPosition, totalLength) / totalLength);
        }
    }
    return 0.0f;
}

bool AudioEngine::isPlaying() const
{
    return engineIsPlaying;
}

void AudioEngine::setBPM(int bpm)
{
    if (bpm > 0 && bpm != currentBPM)
    {
        currentBPM = bpm;
        juce::Logger::writeToLog("AudioEngine: BPM set to " + juce::String(currentBPM));
        listeners.call(&Listener::bpmChanged, currentBPM);
    }
}

void AudioEngine::setKey(const juce::String& key)
{
    if (key.isNotEmpty() && key != currentKey)
    {
        currentKey = key;
        juce::Logger::writeToLog("AudioEngine: Key set to " + currentKey);
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

// --- Dichiarazione della funzione helper interna nel file .h ---
// void AudioEngine::removeTrackAudio_internal(int trackId); // Già dichiarata in .h