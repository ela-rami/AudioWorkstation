#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "UI/FileLoaderComponent.h"

class MainComponent : public juce::Component,
                      public AudioEngine::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // AudioEngine::Listener Implementation
    void fileLoaded(const juce::File& file, double sampleRate, int numChannels) override;
    void playbackStarted() override;
    void playbackStopped() override;
    void bpmChanged(int newBpm) override;
    void keyChanged(const juce::String& newKey) override;

private:
    AudioEngine audioEngine;
    FileLoaderComponent fileLoader;
    
    juce::TextButton playButton;
    juce::TextButton stopButton;
    
    juce::Slider bpmSlider;
    juce::Label bpmLabel;
    
    juce::Label keyLabel;
    juce::TextButton keyUpButton;
    juce::TextButton keyDownButton;
    
    juce::Label statusLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};