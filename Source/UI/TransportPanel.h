#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"

class TransportPanel : public juce::Component
{
public:
    TransportPanel(AudioEngine& engine) : audioEngine(engine)
    {
        // Etichetta progetto
        projectNameLabel.setText("PROJECT-01", juce::dontSendNotification);
        projectNameLabel.setFont(juce::Font().withHeight(16.0f));
        projectNameLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        projectNameLabel.setJustificationType(juce::Justification::centred);
        projectNameLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        addAndMakeVisible(projectNameLabel);
        
        // Controlli BPM - senza etichetta BPM
        bpmEditor.setText("125", juce::dontSendNotification);
        bpmEditor.setFont(juce::Font().withHeight(16.0f));
        bpmEditor.setJustificationType(juce::Justification::centred);
        bpmEditor.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        bpmEditor.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        bpmEditor.setEditable(true);
        bpmEditor.onTextChange = [this] {
            auto bpmValue = bpmEditor.getText().getIntValue();
            if (bpmValue > 0) {
                audioEngine.setBPM(bpmValue);
            }
        };
        addAndMakeVisible(bpmEditor);
        
        // Controlli chiave
        keyEditor.setText("C Minor", juce::dontSendNotification);
        keyEditor.setFont(juce::Font().withHeight(16.0f));
        keyEditor.setJustificationType(juce::Justification::centred);
        keyEditor.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        keyEditor.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        keyEditor.setEditable(true);
        keyEditor.onTextChange = [this] {
            audioEngine.setKey(keyEditor.getText());
        };
        addAndMakeVisible(keyEditor);
        
        // Pulsante Play/Pause
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x20FFFFFF));
        playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4EE6B8));
        addAndMakeVisible(playButton);
        
        // Configura l'icona del pulsante play
        updatePlayButtonIcon(false);
        
        playButton.onClick = [this] {
            isPlaying = !isPlaying;
            updatePlayButtonIcon(isPlaying);
            
            if (isPlaying) {
                audioEngine.play();
            } else {
                audioEngine.stop();
            }
        };
        
        // Volume slider
        volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        volumeSlider.setRange(0.0, 1.0);
        volumeSlider.setValue(0.8);
        volumeSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4EE6B8));
        addAndMakeVisible(volumeSlider);
        
        // Etichetta volume
        volumeLabel.setText("VOL", juce::dontSendNotification);
        volumeLabel.setFont(juce::Font().withHeight(14.0f));
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(volumeLabel);
        
        // Pulsante Save/Export
        saveButton.setButtonText("Save");
        saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4EE6B8));
        saveButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff161626));
        addAndMakeVisible(saveButton);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff161626));
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(12);
        
        // Layout orizzontale
        projectNameLabel.setBounds(bounds.removeFromLeft(180).reduced(3));
        bounds.removeFromLeft(15);
        
        // BPM senza etichetta
        bpmEditor.setBounds(bounds.removeFromLeft(80).reduced(3));
        bounds.removeFromLeft(15);
        
        // Key
        keyEditor.setBounds(bounds.removeFromLeft(120).reduced(3));
        bounds.removeFromLeft(15);
        
        // Play Button
        playButton.setBounds(bounds.removeFromLeft(50).reduced(3));
        bounds.removeFromLeft(25);
        
        // Volume - slider più ampio
        auto volumeBounds = bounds.removeFromLeft(250);
        volumeLabel.setBounds(volumeBounds.removeFromLeft(30));
        volumeSlider.setBounds(volumeBounds);
        bounds.removeFromLeft(25);
        
        // Save Button
        saveButton.setBounds(bounds.removeFromRight(140).reduced(3));
    }
    
    void updatePlayButtonIcon(bool isPlaying)
    {
        playButton.setButtonText(isPlaying ? "||" : ">");
    }
    
private:
    AudioEngine& audioEngine;
    bool isPlaying = false;
    
    juce::Label projectNameLabel;
    juce::Label bpmEditor;
    juce::Label keyEditor;
    
    juce::TextButton playButton;
    
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    
    juce::TextButton saveButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportPanel)
};