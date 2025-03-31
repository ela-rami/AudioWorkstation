#pragma once

#include <JuceHeader.h>
#include <map> // Per std::map

// Assicurati che NON erediti più da juce::ChangeListener
class AudioEngine : public juce::AudioAppComponent
{
public:
    AudioEngine();
    ~AudioEngine() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Metodo per caricare un file associato a un ID di traccia
    bool loadFile(const juce::File& file, int trackId);
    // Metodo per rimuovere l'audio associato a un ID di traccia
    void removeTrackAudio(int trackId);

    void play();
    void stop();

    // Ottiene la posizione relativa per una specifica traccia
    float getPositionRelative(int trackId) const;
    bool isPlaying() const; // Controlla se l'engine sta suonando

    // --- Gestione BPM e Chiave (Implementazione base) ---
    int getCurrentBPM() const { return currentBPM; }
    juce::String getCurrentKey() const { return currentKey; }
    void setBPM(int bpm);
    void setKey(const juce::String& key);

    // --- Listener per notifiche (es. file caricato, BPM/Key cambiati) ---
    class Listener
    {
    public:
        virtual ~Listener() = default;
        // Notifica file caricato per una traccia specifica
        virtual void fileLoaded(const juce::File& file, int trackId) {}
        // Notifica cambio BPM
        virtual void bpmChanged(int newBpm) {}
        // Notifica cambio Chiave
        virtual void keyChanged(const juce::String& newKey) {}
        // Potresti aggiungere altri callback se necessario
    };

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    // Dichiarazione della funzione helper interna
    void removeTrackAudio_internal(int trackId);

    // Struttura interna per tenere insieme le risorse audio di una traccia
    struct TrackAudioSource
    {
        std::unique_ptr<juce::AudioFormatReader> reader; // Teniamo il reader per info
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        std::unique_ptr<juce::AudioTransportSource> transportSource;

        // Costruttore di default per permettere l'inserimento nella mappa prima dell'inizializzazione completa
         TrackAudioSource() = default;

         // Costruttore di spostamento per std::move
         TrackAudioSource(TrackAudioSource&&) = default;
         TrackAudioSource& operator=(TrackAudioSource&&) = default;

         // Impedisci copia
         TrackAudioSource(const TrackAudioSource&) = delete;
         TrackAudioSource& operator=(const TrackAudioSource&) = delete;
    };

    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread thread { "Audio Load Thread" }; // Thread per lettura file
    juce::MixerAudioSource mixerSource;                  // Mixer per combinare le tracce

    // Mappa che associa l'ID della traccia (int) alle sue risorse audio
    std::map<int, TrackAudioSource> trackSources;
    juce::CriticalSection sourceLock; // Lock per proteggere l'accesso a trackSources e mixerSource

    int currentBPM = 120;
    juce::String currentKey = "C Minor"; // Chiave iniziale
    std::atomic<bool> engineIsPlaying { false }; // Stato di riproduzione globale (thread-safe)

    juce::ListenerList<Listener> listeners; // Lista dei listener dell'AudioEngine

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};