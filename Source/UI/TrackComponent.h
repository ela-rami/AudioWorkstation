#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"

class TrackComponent : public juce::Component,
                       public juce::Timer
{
public:
    TrackComponent(int trackIndex, AudioEngine& engine)
        : trackNumber(trackIndex), audioEngine(engine), isMouseOver(false)
    {
        // Generare un colore basato sull'indice traccia
        trackColour = getColourForTrackIndex(trackIndex);
        
        // Etichetta nome file
        fileNameLabel.setText("No file loaded", juce::dontSendNotification);
        fileNameLabel.setFont(juce::Font().withStyle(juce::Font::bold).withHeight(18.0f));
        fileNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        fileNameLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(fileNameLabel);
        
        // Etichetta info file
        fileInfoLabel.setText("", juce::dontSendNotification);
        fileInfoLabel.setFont(juce::Font().withHeight(14.0f));
        fileInfoLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFBBBBBB));
        fileInfoLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(fileInfoLabel);
        
        // Slider volume - ALLUNGATO
        volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        volumeSlider.setRange(0.0, 1.0);
        volumeSlider.setValue(0.8);
        volumeSlider.setColour(juce::Slider::trackColourId, trackColour);
        addAndMakeVisible(volumeSlider);
        
        // Pulsanti controllo - Corretti con testi ASCII standard
        configureButton(muteButton, "M");
        configureButton(soloButton, "S");
        configureButton(eqButton, "...");  // Per adattarsi meglio ai bottoni tondi
        configureButton(deleteButton, "X");  // Per eliminare la traccia
        
        // Etichetta volume
        volumeLabel.setText("VOL", juce::dontSendNotification);
        volumeLabel.setFont(juce::Font().withHeight(14.0f));
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(volumeLabel);
        
        // Aggiungi listener per i pulsanti
        deleteButton.onClick = [this] { notifyRemoval(); };
        
        // Avvia timer per aggiornamenti UI
        startTimerHz(30);
        
        // Per il mouse hover
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    ~TrackComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        // Sfondo base della traccia
        g.setColour(juce::Colour(0xff252537).withAlpha(0.5f));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
        
        // Area di progresso con gradiente
        if (audioFile.existsAsFile())
        {
            // Crea un gradiente per l'area di progresso
            float progress = audioEngine.getPositionRelative();
            if (progress <= 0.0f) progress = 0.5f; // Per test UI mockup
            
            auto progressBounds = getLocalBounds().toFloat().withWidth(getWidth() * progress);
            
            juce::ColourGradient gradient(
                trackColour.withAlpha(0.8f), progressBounds.getX(), progressBounds.getCentreY(),
                trackColour.withAlpha(0.3f), progressBounds.getRight(), progressBounds.getCentreY(),
                false);
                
            g.setGradientFill(gradient);
            g.fillRoundedRectangle(progressBounds, 10.0f);
            
            // Disegna cerchio con numero traccia
            auto circleBounds = juce::Rectangle<float>(70, 70).withCentre(juce::Point<float>(45, getHeight() / 2.0f));
            g.setColour(trackColour);
            g.fillEllipse(circleBounds);
            
            g.setColour(juce::Colours::black);
            g.setFont(juce::Font().withStyle(juce::Font::bold).withHeight(24.0f));
            g.drawText(juce::String(trackNumber), circleBounds, juce::Justification::centred);
        }
        else
        {
            // Se nessun file è caricato, mostra un cerchio con bordo tratteggiato
            auto circleBounds = juce::Rectangle<float>(70, 70).withCentre(juce::Point<float>(45, getHeight() / 2.0f));
            
            juce::Path dashedCircle;
            dashedCircle.addEllipse(circleBounds);
            
            juce::PathStrokeType strokeType(2.0f);
            float dashedArray[] = { 5.0f, 5.0f };
            
            g.setColour(juce::Colours::lightgrey.withAlpha(0.5f));
            g.strokePath(dashedCircle, strokeType, juce::AffineTransform().translated(circleBounds.getX(), circleBounds.getY()));
            
            g.setFont(juce::Font().withStyle(juce::Font::bold).withHeight(24.0f));
            g.drawText(juce::String(trackNumber), circleBounds, juce::Justification::centred);
            
            // Messaggio "Drop audio"
            g.setFont(juce::Font().withHeight(16.0f));
            g.drawText("Drop audio file here", getLocalBounds().reduced(80, 0), juce::Justification::centred);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        
        // Area cerchio
        bounds.removeFromLeft(80);
        
        // Etichette
        auto textArea = bounds.removeFromLeft(bounds.getWidth() / 2);
        fileNameLabel.setBounds(textArea.removeFromTop(30));
        fileInfoLabel.setBounds(textArea.removeFromTop(30));
        
        // Pulsanti e controlli
        if (isMouseOver || true) // Per test mockup, mostra sempre i controlli
        {
            // Area pulsanti 
            auto controlsArea = bounds;
            
            // Volume - ALLUNGATO
            auto volumeArea = controlsArea.removeFromLeft(200);  // Ampio volume slider
            volumeLabel.setBounds(volumeArea.removeFromLeft(30).withHeight(30).withY(bounds.getCentreY() - 15));
            volumeSlider.setBounds(volumeArea.withHeight(30).withY(bounds.getCentreY() - 15));
            
            // Pulsanti
            auto buttonArea = controlsArea.removeFromRight(200);
            deleteButton.setBounds(buttonArea.removeFromRight(40).reduced(5).withHeight(30).withY(bounds.getCentreY() - 15));
            eqButton.setBounds(buttonArea.removeFromRight(40).reduced(5).withHeight(30).withY(bounds.getCentreY() - 15));
            soloButton.setBounds(buttonArea.removeFromRight(40).reduced(5).withHeight(30).withY(bounds.getCentreY() - 15));
            muteButton.setBounds(buttonArea.removeFromRight(40).reduced(5).withHeight(30).withY(bounds.getCentreY() - 15));
        }
    }
    
    void mouseEnter(const juce::MouseEvent& event) override
    {
        isMouseOver = true;
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent& event) override
    {
        isMouseOver = false;
        repaint();
    }
    
    void timerCallback() override
    {
        // Per aggiornare la visualizzazione del progresso
        repaint();
    }
    
    bool loadFile(const juce::File& file)
    {
        audioFile = file;
        fileNameLabel.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);
        
        // Impostazione dell'etichetta info
        juce::String infoText = file.getFileName();
        fileInfoLabel.setText(infoText, juce::dontSendNotification);
        
        repaint();
        return true;
    }
    
    juce::File getAudioFile() const
    {
        return audioFile;
    }
    
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void trackRemovalRequested(TrackComponent* track) = 0;
    };
    
    void addListener(Listener* listener)
    {
        listeners.add(listener);
    }
    
    void removeListener(Listener* listener)
    {
        listeners.remove(listener);
    }
    
private:
    void notifyRemoval()
    {
        listeners.call(&Listener::trackRemovalRequested, this);
    }
    
    void configureButton(juce::TextButton& button, const juce::String& text)
    {
        button.setButtonText(text);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0x20FFFFFF));
        button.setColour(juce::TextButton::buttonOnColourId, trackColour);
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        button.setClickingTogglesState(true);
        addAndMakeVisible(button);
    }
    
    juce::Colour getColourForTrackIndex(int index) const
    {
        // Genera un colore basato sull'indice traccia
        switch (index % 5)
        {
            case 0: return juce::Colour(0xFF4EE6B8); // Verde acqua
            case 1: return juce::Colour(0xFFE64E88); // Rosa
            case 2: return juce::Colour(0xFF4E88E6); // Blu
            case 3: return juce::Colour(0xFFE6B84E); // Arancione 
            case 4: return juce::Colour(0xFFB84EE6); // Viola
            default: return juce::Colour(0xFF4EE6B8);
        }
    }
    
    int trackNumber;
    AudioEngine& audioEngine;
    juce::File audioFile;
    juce::Colour trackColour;
    bool isMouseOver;
    
    juce::Label fileNameLabel;
    juce::Label fileInfoLabel;
    
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    
    juce::TextButton muteButton;
    juce::TextButton soloButton;
    juce::TextButton eqButton;
    juce::TextButton deleteButton;
    
    juce::ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};