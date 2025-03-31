#pragma once

#include <JuceHeader.h>
#include <vector> // Necessario per std::vector o juce::PathStrokeType::DashLengths
#include "../Audio/AudioEngine.h"

class TrackComponent : public juce::Component,
                       public juce::Timer
{
public:
    TrackComponent(int trackIndex, AudioEngine& engine)
        : trackNumber(trackIndex), audioEngine(engine), isMouseOver(false)
    {
        trackColour = getColourForTrackIndex(trackIndex);

        // --- Configurazione UI originale ---
        fileNameLabel.setFont(juce::Font(18.0f).withStyle(juce::Font::bold));
        fileNameLabel.setText("No file loaded", juce::dontSendNotification);
        fileNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        fileNameLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(fileNameLabel);

        fileInfoLabel.setFont(juce::Font(14.0f));
        fileInfoLabel.setText("", juce::dontSendNotification);
        fileInfoLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFBBBBBB));
        fileInfoLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(fileInfoLabel);

        volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        volumeSlider.setRange(0.0, 1.0, 0.01);
        volumeSlider.setValue(0.8);
        volumeSlider.setColour(juce::Slider::trackColourId, trackColour);
        addAndMakeVisible(volumeSlider);

        volumeLabel.setFont(juce::Font(14.0f));
        volumeLabel.setText("VOL", juce::dontSendNotification);
        volumeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(volumeLabel);

        configureButton(muteButton, "M");
        configureButton(soloButton, "S");
        // Ripristino testo originale se preferito
        configureButton(eqButton, "..."); // O "EQ"
        configureButton(deleteButton, "X");

        deleteButton.onClick = [this] { notifyRemoval(); };

        startTimerHz(30);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    ~TrackComponent() override
    {
        stopTimer();
    }

    // --- Paint con UI originale e fix Dash ---
    void paint(juce::Graphics& g) override
    {
        // Sfondo base della traccia
        g.setColour(juce::Colour(0xff252537).withAlpha(0.5f)); // Ripristino sfondo originale
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

        if (audioFile.existsAsFile())
        {
             // --- Area di progresso originale ---
             float progress = audioEngine.getPositionRelative(trackNumber);
             // NB: Rimosso il mockup progress = 0.5f;
             // if (progress <= 0.0f) progress = 0.5f; // Per test UI mockup

             auto progressBounds = getLocalBounds().toFloat().withWidth(getWidth() * progress);

             // Se il progresso è quasi zero, disegna almeno un minimo per visibilità (Opzionale)
             // if (progress > 0.0f && progressBounds.getWidth() < 2.0f)
             //     progressBounds.setWidth(2.0f);

             juce::ColourGradient gradient(
                 trackColour.withAlpha(0.8f), progressBounds.getX(), progressBounds.getCentreY(),
                 trackColour.withAlpha(0.3f), progressBounds.getRight(), progressBounds.getCentreY(),
                 false);

             g.setGradientFill(gradient);
             // Applica il gradiente solo se c'è progresso > 0
             if (progress > 0.0f)
                  g.fillRoundedRectangle(progressBounds, 10.0f);


            // --- Cerchio con numero traccia originale ---
            auto circleBounds = juce::Rectangle<float>(70, 70).withCentre(juce::Point<float>(45, getHeight() / 2.0f));
            g.setColour(trackColour);
            g.fillEllipse(circleBounds);

            g.setColour(juce::Colours::black);
            g.setFont(juce::Font(24.0f).withStyle(juce::Font::bold));
            g.drawText(juce::String(trackNumber), circleBounds, juce::Justification::centred);
        }
        else
        {
            // --- Cerchio vuoto originale con FIX per dash ---
            auto circleBounds = juce::Rectangle<float>(70, 70).withCentre(juce::Point<float>(45, getHeight() / 2.0f));
            juce::Path dashedCircle;
            dashedCircle.addEllipse(circleBounds);

            // CORREZIONE DEFINITIVA: Crea PathStrokeType semplice e passa il pattern a strokePath
            juce::PathStrokeType strokeType(2.0f); // Solo spessore
            std::vector<float> dashLengths { 5.0f, 5.0f }; // Definisci il pattern

            g.setColour(juce::Colours::lightgrey.withAlpha(0.5f));
            // Passa dashLengths come ultimo argomento a strokePath
            juce::Path dashedPath;
            strokeType.createDashedStroke(dashedPath, dashedCircle, dashLengths.data(), dashLengths.size());
            g.strokePath(dashedPath, strokeType);

            g.setFont(juce::Font(24.0f).withStyle(juce::Font::bold));
            g.drawText(juce::String(trackNumber), circleBounds, juce::Justification::centred);

            // Messaggio "Drop audio" originale
            g.setFont(juce::Font(16.0f));
            g.setColour(juce::Colours::lightgrey.withAlpha(0.8f)); // Mantengo colore leggibile
            g.drawText("Drop audio file here", getLocalBounds().reduced(80, 0), juce::Justification::centred); // Giustificazione originale
        }
    }

    // --- Resized originale ---
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);

        // Area cerchio (spazio vuoto a sinistra)
        bounds.removeFromLeft(80);

        // Testo a sinistra
        auto textArea = bounds.removeFromLeft(bounds.getWidth() / 2);
        fileNameLabel.setBounds(textArea.removeFromTop(30));
        fileInfoLabel.setBounds(textArea.removeFromTop(30));

        // Controlli a destra
        // Usa la logica isMouseOver originale se vuoi nascondere/mostrare
        // if (isMouseOver || true) // Metti false qui se vuoi controlli solo on hover
        {
            auto controlsArea = bounds; // Area rimanente per i controlli

            // Slider volume allungato
            auto volumeArea = controlsArea.removeFromLeft(200);
            volumeLabel.setBounds(volumeArea.removeFromLeft(30).withHeight(30).withY(bounds.getCentreY() - 15));
            volumeSlider.setBounds(volumeArea.withHeight(30).withY(bounds.getCentreY() - 15));

            // Pulsanti a destra
            auto buttonArea = controlsArea.removeFromRight(200); // Stima larghezza area bottoni
            int buttonWidth = 40;
            int buttonHeight = 30;
            int buttonY = bounds.getCentreY() - buttonHeight / 2;
            int buttonSpacing = 5; // Spazio tra i bottoni

            deleteButton.setBounds(buttonArea.removeFromRight(buttonWidth).reduced(buttonSpacing).withHeight(buttonHeight).withY(buttonY));
            eqButton.setBounds(buttonArea.removeFromRight(buttonWidth).reduced(buttonSpacing).withHeight(buttonHeight).withY(buttonY));
            soloButton.setBounds(buttonArea.removeFromRight(buttonWidth).reduced(buttonSpacing).withHeight(buttonHeight).withY(buttonY));
            muteButton.setBounds(buttonArea.removeFromRight(buttonWidth).reduced(buttonSpacing).withHeight(buttonHeight).withY(buttonY));

            // Rendi visibili i controlli se necessario (basato su isMouseOver)
            bool showControls = isMouseOver || true; // Modifica qui se vuoi controlli solo on hover
            volumeLabel.setVisible(showControls);
            volumeSlider.setVisible(showControls);
            deleteButton.setVisible(showControls);
            eqButton.setVisible(showControls);
            soloButton.setVisible(showControls);
            muteButton.setVisible(showControls);
        }
        /* else { // Opzionale: nascondi i controlli
             volumeLabel.setVisible(false);
             volumeSlider.setVisible(false);
             deleteButton.setVisible(false);
             eqButton.setVisible(false);
             soloButton.setVisible(false);
             muteButton.setVisible(false);
        } */
    }


    void mouseEnter(const juce::MouseEvent& event) override
    {
        isMouseOver = true;
        resized(); // Chiama resized per aggiornare la visibilità dei controlli (se implementato)
        // repaint(); // Potrebbe servire anche repaint se resized non basta
    }

    void mouseExit(const juce::MouseEvent& event) override
    {
        isMouseOver = false;
        resized(); // Chiama resized per aggiornare la visibilità dei controlli (se implementato)
        // repaint(); // Potrebbe servire anche repaint se resized non basta
    }

    void timerCallback() override
    {
        if (audioFile.existsAsFile())
        {
            repaint(); // Aggiorna la barra di progresso
        }
    }

    bool loadFile(const juce::File& file)
    {
        if (file.existsAsFile())
        {
            audioFile = file;
            fileNameLabel.setText(file.getFileNameWithoutExtension(), juce::dontSendNotification);

            juce::AudioFormatManager tempFormatManager;
            tempFormatManager.registerBasicFormats();
            std::unique_ptr<juce::AudioFormatReader> reader(tempFormatManager.createReaderFor(file));
            if(reader != nullptr)
            {
                 double duration = reader->lengthInSamples / reader->sampleRate;
                 int minutes = static_cast<int>(duration / 60.0);
                 int seconds = static_cast<int>(std::fmod(duration, 60.0));
                 juce::String durationStr = juce::String::formatted("%d:%02d", minutes, seconds);
                 // Ripristino testo info originale
                 juce::String infoText = file.getFileName(); // O le info dettagliate:
                 // juce::String infoText = juce::String(reader->sampleRate, 0) + " Hz, " + juce::String(reader->numChannels) + " ch, " + durationStr;
                 fileInfoLabel.setText(infoText, juce::dontSendNotification);
            }
            else
            {
                 fileInfoLabel.setText("Info not available", juce::dontSendNotification);
            }

            repaint();
            return true;
        }
        return false;
    }

    juce::File getAudioFile() const { return audioFile; }
    int getTrackNumber() const { return trackNumber; }

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void trackRemovalRequested(TrackComponent* track) = 0;
    };
    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

private:
    void notifyRemoval() { listeners.call(&Listener::trackRemovalRequested, this); }

    // --- ConfigureButton originale ---
    void configureButton(juce::TextButton& button, const juce::String& text)
    {
        button.setButtonText(text);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0x20FFFFFF)); // Colore originale
        button.setColour(juce::TextButton::buttonOnColourId, trackColour);             // Colore originale
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        button.setClickingTogglesState(text == "M" || text == "S");
        // Rimosso setConnectedEdges se non faceva parte dell'originale
        // button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft | juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
        addAndMakeVisible(button);
    }

    // --- GetColourForTrackIndex originale ---
    juce::Colour getColourForTrackIndex(int index) const
    {
        // Genera un colore basato sull'indice traccia (logica originale)
        switch (index % 5)
        {
            case 0: return juce::Colour(0xFF4EE6B8); // Verde acqua
            case 1: return juce::Colour(0xFFE64E88); // Rosa
            case 2: return juce::Colour(0xFF4E88E6); // Blu
            case 3: return juce::Colour(0xFFE6B84E); // Arancione
            case 4: return juce::Colour(0xFFB84EE6); // Viola
            default: return juce::Colour(0xFF4EE6B8);
        }
        // Se preferivi il colore casuale:
        // juce::Random randomGenerator(index);
        // return juce::Colour::fromHSV(randomGenerator.nextFloat(),
        //                              0.6f + randomGenerator.nextFloat() * 0.3f,
        //                              0.7f + randomGenerator.nextFloat() * 0.2f,
        //                              1.0f);
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