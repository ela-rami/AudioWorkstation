#include "MainComponent.h"

MainComponent::MainComponent()
    : fileLoader(audioEngine)
{
    // Register this component as a listener of the audio engine
    audioEngine.addListener(this);
    
    // Add and configure user interface components
    addAndMakeVisible(fileLoader);
    
    // Playback buttons
    addAndMakeVisible(playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { audioEngine.play(); };
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { audioEngine.stop(); };
    
    // BPM control
    addAndMakeVisible(bpmSlider);
    bpmSlider.setRange(60.0, 200.0, 1.0);
    bpmSlider.setValue(120.0);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    bpmSlider.onValueChange = [this] { audioEngine.setBPM(static_cast<int>(bpmSlider.getValue())); };
    
    addAndMakeVisible(bpmLabel);
    bpmLabel.setText("BPM:", juce::dontSendNotification);
    bpmLabel.attachToComponent(&bpmSlider, true);
    
    // Key control
    addAndMakeVisible(keyLabel);
    keyLabel.setText("Key: C", juce::dontSendNotification);
    
    addAndMakeVisible(keyUpButton);
    keyUpButton.setButtonText("▲");
    keyUpButton.onClick = [this] { 
        // Implement key change logic
    };
    
    addAndMakeVisible(keyDownButton);
    keyDownButton.setButtonText("▼");
    keyDownButton.onClick = [this] { 
        // Implement key change logic
    };
    
    // Status label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Load an audio file to start", juce::dontSendNotification);
    
    // Set component size
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    audioEngine.removeListener(this);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    
    // File loading area
    fileLoader.setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(20);
    
    // Transport controls
    auto transportBounds = bounds.removeFromTop(40);
    playButton.setBounds(transportBounds.removeFromLeft(100));
    transportBounds.removeFromLeft(10);
    stopButton.setBounds(transportBounds.removeFromLeft(100));
    bounds.removeFromTop(20);
    
    // BPM controls
    auto bpmBounds = bounds.removeFromTop(40);
    bpmSlider.setBounds(bpmBounds.withTrimmedLeft(50));
    bounds.removeFromTop(20);
    
    // Key controls
    auto keyBounds = bounds.removeFromTop(40);
    keyLabel.setBounds(keyBounds.removeFromLeft(120));
    keyUpButton.setBounds(keyBounds.removeFromLeft(40));
    keyBounds.removeFromLeft(10);
    keyDownButton.setBounds(keyBounds.removeFromLeft(40));
    bounds.removeFromTop(20);
    
    // Status label
    statusLabel.setBounds(bounds.removeFromTop(30));
}

void MainComponent::fileLoaded(const juce::File& file, double sampleRate, int numChannels)
{
    statusLabel.setText("Loaded: " + file.getFileName() + " (" + 
                       juce::String(numChannels) + " channels, " + 
                       juce::String(sampleRate) + " Hz)", 
                       juce::dontSendNotification);
}

void MainComponent::playbackStarted()
{
    statusLabel.setText("Playing", juce::dontSendNotification);
}

void MainComponent::playbackStopped()
{
    statusLabel.setText("Stopped", juce::dontSendNotification);
}

void MainComponent::bpmChanged(int newBpm)
{
    bpmSlider.setValue(newBpm, juce::dontSendNotification);
}

void MainComponent::keyChanged(const juce::String& newKey)
{
    keyLabel.setText("Key: " + newKey, juce::dontSendNotification);
}