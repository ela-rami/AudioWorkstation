#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"

class TransportPanel : public juce::Component, public AudioEngine::Listener
{
public:
    TransportPanel(AudioEngine& engine) : audioEngine(engine)
    {
        audioEngine.addListener(this);

        // CORREZIONE: Sintassi Font moderna (già ok per nome+stile)
        projectNameLabel.setFont(juce::Font("Poppins", 16.0f, juce::Font::bold));
        projectNameLabel.setText("PROJECT-DAW", juce::dontSendNotification);
        projectNameLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        projectNameLabel.setJustificationType(juce::Justification::centred);
        projectNameLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        projectNameLabel.setBorderSize({0, 0, 0, 0});
        addAndMakeVisible(projectNameLabel);

        // CORREZIONE: Sintassi Font moderna (già ok per nome+stile)
        bpmEditor.setFont(juce::Font("Poppins", 16.0f, juce::Font::plain));
        bpmEditor.setText(juce::String(audioEngine.getCurrentBPM()), juce::dontSendNotification);
        bpmEditor.setJustificationType(juce::Justification::centred);
        bpmEditor.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        bpmEditor.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        bpmEditor.setEditable(true, true, false);
        bpmEditor.onTextChange = [this] {
            int bpmValue = bpmEditor.getText().getIntValue();
            if (bpmValue > 20 && bpmValue < 400) {
                audioEngine.setBPM(bpmValue);
            } else {
                bpmEditor.setText(juce::String(audioEngine.getCurrentBPM()), juce::dontSendNotification);
            }
        };
        addAndMakeVisible(bpmEditor);

        // CORREZIONE: Sintassi Font moderna e stile 'plain'
        bpmLabel.setFont(juce::Font("Poppins", 10.0f, juce::Font::plain)); // Usa plain invece di light
        bpmLabel.setText("BPM", juce::dontSendNotification);
        bpmLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        bpmLabel.setJustificationType(juce::Justification::centredTop);
        addAndMakeVisible(bpmLabel);

        // CORREZIONE: Sintassi Font moderna (già ok per nome+stile)
        keyEditor.setFont(juce::Font("Poppins", 16.0f, juce::Font::plain));
        keyEditor.setText(audioEngine.getCurrentKey(), juce::dontSendNotification);
        keyEditor.setJustificationType(juce::Justification::centred);
        keyEditor.setColour(juce::Label::backgroundColourId, juce::Colour(0x20FFFFFF));
        keyEditor.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        keyEditor.setEditable(true, true, false);
        keyEditor.onTextChange = [this] {
            audioEngine.setKey(keyEditor.getText());
        };
        addAndMakeVisible(keyEditor);

        // CORREZIONE: Sintassi Font moderna e stile 'plain'
        keyLabel.setFont(juce::Font("Poppins", 10.0f, juce::Font::plain)); // Usa plain invece di light
        keyLabel.setText("KEY", juce::dontSendNotification);
        keyLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        keyLabel.setJustificationType(juce::Justification::centredTop);
        addAndMakeVisible(keyLabel);

        playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x30FFFFFF));
        playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4EE6B8));
        playButton.setClickingTogglesState(false);
        // CORREZIONE: Rimuovi setCornerRadius
        // playButton.setCornerRadius(20.f);
        addAndMakeVisible(playButton);

        updatePlayButtonIcon(audioEngine.isPlaying());

        playButton.onClick = [this] {
            if (audioEngine.isPlaying()) {
                audioEngine.stop();
            } else {
                audioEngine.play();
            }
            updatePlayButtonIcon(audioEngine.isPlaying());
        };

        volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        volumeSlider.setRange(0.0, 1.0, 0.01);
        volumeSlider.setValue(0.8);
        volumeSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4EE6B8));
        volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        addAndMakeVisible(volumeSlider);

        // CORREZIONE: Sintassi Font moderna e stile 'plain'
        volumeLabel.setFont(juce::Font("Poppins", 10.0f, juce::Font::plain)); // Usa plain invece di light
        volumeLabel.setText("MASTER", juce::dontSendNotification);
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        volumeLabel.setJustificationType(juce::Justification::centredTop);
        addAndMakeVisible(volumeLabel);

        saveButton.setButtonText("SAVE");
        saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4EE6B8));
        saveButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff161626));
        // CORREZIONE: Rimuovi setCornerRadius
        // saveButton.setCornerRadius(5.f);
        addAndMakeVisible(saveButton);
    }

     ~TransportPanel() override
     {
         audioEngine.removeListener(this);
     }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff1C1C2E));
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawLine(0.0f, (float)getHeight() - 1.0f, (float)getWidth(), (float)getHeight() - 1.0f, 1.0f);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15, 10);
        int spacing = 20;
        int smallSpacing = 5;

        projectNameLabel.setBounds(bounds.removeFromLeft(180));
        bounds.removeFromLeft(spacing);

        auto bpmArea = bounds.removeFromLeft(80);
        bpmLabel.setBounds(bpmArea.removeFromBottom(15));
        bpmArea.removeFromBottom(smallSpacing);
        bpmEditor.setBounds(bpmArea);
        bounds.removeFromLeft(spacing);

        auto keyArea = bounds.removeFromLeft(120);
        keyLabel.setBounds(keyArea.removeFromBottom(15));
        keyArea.removeFromBottom(smallSpacing);
        keyEditor.setBounds(keyArea);
        bounds.removeFromLeft(spacing + 10);

        playButton.setBounds(bounds.removeFromLeft(40).withSizeKeepingCentre(40, 40));
        bounds.removeFromLeft(spacing + 10);

        saveButton.setBounds(bounds.removeFromRight(100));
        bounds.removeFromRight(spacing);

        auto volumeArea = bounds;
        volumeLabel.setBounds(volumeArea.removeFromBottom(15));
        volumeArea.removeFromBottom(smallSpacing);
        volumeSlider.setBounds(volumeArea);
    }

    void bpmChanged(int newBpm) override {
         if (bpmEditor.getText().getIntValue() != newBpm) {
             bpmEditor.setText(juce::String(newBpm), juce::dontSendNotification);
         }
     }
     void keyChanged(const juce::String& newKey) override {
         if (keyEditor.getText() != newKey) {
             keyEditor.setText(newKey, juce::dontSendNotification);
         }
     }

    void updatePlayButtonIcon(bool engineIsPlaying)
    {
        playButton.setButtonText(engineIsPlaying ? juce::String(L"\u23F8") : juce::String(L"\u25B6"));
        playButton.setToggleState(engineIsPlaying, juce::dontSendNotification);
    }

private:
    AudioEngine& audioEngine;

    juce::Label projectNameLabel;
    juce::Label bpmEditor;
    juce::Label bpmLabel;
    juce::Label keyEditor;
    juce::Label keyLabel;
    juce::TextButton playButton;
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::TextButton saveButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportPanel)
};