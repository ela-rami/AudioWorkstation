#pragma once

#include <JuceHeader.h>

class AudioEngine : public juce::AudioAppComponent,
                    public juce::ChangeListener
{
public:
    AudioEngine();
    ~AudioEngine() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    bool loadFile(const juce::File& file);
    void play();
    void stop();
    
    float getPositionRelative() const;
    int getCurrentBPM() const { return currentBPM; }
    juce::String getCurrentKey() const { return currentKey; }
    
    void setBPM(int bpm);
    void setKey(const juce::String& key);
    
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void fileLoaded(const juce::File& file, double sampleRate, int numChannels) {}
        virtual void playbackStarted() {}
        virtual void playbackStopped() {}
        virtual void bpmChanged(int newBpm) {}
        virtual void keyChanged(const juce::String& newKey) {}
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread thread{"AudioFileReaderThread"};
    
    int currentBPM = 120;
    juce::String currentKey = "C";
    
    juce::ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};