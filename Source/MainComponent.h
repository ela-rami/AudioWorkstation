#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "UI/ModernLookAndFeel.h"
#include "UI/TrackComponent.h"
#include "UI/SidebarComponent.h"
#include "UI/TransportPanel.h"

class MainComponent : public juce::Component,
                      public juce::FileDragAndDropTarget,
                      public AudioEngine::Listener,
                      public TrackComponent::Listener,
                      public SidebarComponent::Listener
{
public:
    MainComponent()
    {
        // Impostazione del look and feel personalizzato
        lookAndFeel = std::make_unique<ModernLookAndFeel>();
        juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get());
        
        // Registra questo componente come ascoltatore dell'audio engine
        audioEngine.addListener(this);
        
        // Configura la sidebar
        sidebar.addListener(this);
        addAndMakeVisible(sidebar);
        
        // Configura il pannello trasporto
        addAndMakeVisible(transportPanel);
        
        // Configura il contenitore delle tracce
        tracksViewport.setViewedComponent(&tracksContainer, false);
        tracksViewport.setScrollBarsShown(true, false);
        addAndMakeVisible(tracksViewport);
        
        // Pulsante aggiungi traccia
        addTrackButton.setButtonText("+ Add Track");
        addTrackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x20FFFFFF));
        addTrackButton.onClick = [this] { addNewTrack(); };
        addAndMakeVisible(addTrackButton);
        
        // Impostazione delle dimensioni di default - AUMENTATE PER DARE PIÙ SPAZIO
        setSize(1600, 900);
    }

    ~MainComponent() override
    {
        audioEngine.removeListener(this);
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        // Gradiente di sfondo
        juce::ColourGradient gradient(
            juce::Colour(0xff1a1a2e), 0.0f, 0.0f,
            juce::Colour(0xff121212), static_cast<float>(getWidth()), static_cast<float>(getHeight()),
            false);
            
        g.setGradientFill(gradient);
        g.fillAll();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Sidebar (larghezza fissa)
        sidebar.setBounds(0, 0, sidebarWidth, bounds.getHeight());
        
        // Area principale
        auto mainArea = bounds.withTrimmedLeft(sidebarWidth);
        
        // Pannello trasporto
        transportPanel.setBounds(mainArea.removeFromTop(60));
        
        // Pulsante Add Track
        auto addTrackButtonBounds = mainArea.removeFromBottom(60);
        addTrackButton.setBounds(juce::Rectangle<int>(180, 40)
                                .withCentre(addTrackButtonBounds.getCentre()));
        
        // Contenitore tracce
        tracksViewport.setBounds(mainArea);
        
        // Aggiorna le dimensioni del contenitore delle tracce
        updateTracksLayout();
    }

    bool isInterestedInFileDrag(const juce::StringArray& files) override
    {
        // Accetta solo file audio
        for (auto& file : files)
        {
            if (file.endsWithIgnoreCase(".wav") || 
                file.endsWithIgnoreCase(".mp3") || 
                file.endsWithIgnoreCase(".aiff") || 
                file.endsWithIgnoreCase(".ogg"))
                return true;
        }
        
        return false;
    }

    void filesDropped(const juce::StringArray& files, int x, int y) override
    {
        // Converti le coordinate dal sistema di questo componente a quello del contenitore tracce
        juce::Point<int> relativePoint = getLocalPoint(&tracksContainer, juce::Point<int>(x, y));
        
        // Trova la traccia sotto il punto di drop, o crea una nuova traccia
        TrackComponent* targetTrack = nullptr;
        
        for (auto* track : tracks)
        {
            if (track->getBounds().contains(relativePoint))
            {
                targetTrack = track;
                break;
            }
        }
        
        if (targetTrack == nullptr)
        {
            // Se non è stato trovato un target, aggiungi una nuova traccia
            targetTrack = addNewTrack();
        }
        
        // Carica il file nella traccia
        if (targetTrack != nullptr && files.size() > 0)
        {
            juce::File file(files[0]);
            if (file.existsAsFile())
            {
                targetTrack->loadFile(file);
                audioEngine.loadFile(file);
            }
        }
    }

    // AudioEngine::Listener
    void fileLoaded(const juce::File& file, double sampleRate, int numChannels) override
    {
        // Se non abbiamo tracce, crea una nuova traccia e carica il file
        if (tracks.isEmpty())
        {
            auto* track = addNewTrack();
            track->loadFile(file);
        }
    }

    void playbackStarted() override
    {
        transportPanel.updatePlayButtonIcon(true);
    }

    void playbackStopped() override
    {
        transportPanel.updatePlayButtonIcon(false);
    }

    // TrackComponent::Listener
    void trackRemovalRequested(TrackComponent* track) override
    {
        // Trova l'indice della traccia
        int trackIndex = tracks.indexOf(track);
        
        if (trackIndex >= 0)
        {
            // Rimuovi la traccia dalla lista
            tracks.remove(trackIndex);
            
            // Rinumera le tracce rimanenti
            for (int i = 0; i < tracks.size(); ++i)
            {
                // Dovresti avere un metodo per aggiornare il numero della traccia
                // Ad esempio: tracks[i]->setTrackNumber(i + 1);
                // Per ora, ricrea semplicemente la traccia
                delete tracks[i];
                tracks.set(i, nullptr);
                
                auto* newTrack = new TrackComponent(i + 1, audioEngine);
                newTrack->addListener(this);
                tracks.set(i, newTrack);
                tracksContainer.addAndMakeVisible(newTrack);
            }
            
            // Aggiorna il layout
            updateTracksLayout();
        }
    }

    // SidebarComponent::Listener
    void sidebarToggleRequested() override
    {
        // Toggle della sidebar (collassa/espandi)
        sidebarCollapsed = !sidebarCollapsed;
        sidebarWidth = sidebarCollapsed ? 60 : 220;
        resized(); // Ridimensiona tutto
    }

private:
    TrackComponent* addNewTrack()
    {
        auto* track = new TrackComponent(tracks.size() + 1, audioEngine);
        track->addListener(this);
        tracks.add(track);
        tracksContainer.addAndMakeVisible(track);
        
        updateTracksLayout();
        
        return track;
    }
    
    void updateTracksLayout()
    {
        const int trackHeight = 140;
        const int trackSpacing = 10;
        
        // Posiziona le tracce verticalmente
        int yPos = 10;
        for (auto* track : tracks)
        {
            track->setBounds(10, yPos, tracksViewport.getMaximumVisibleWidth() - 20, trackHeight);
            yPos += trackHeight + trackSpacing;
        }
        
        // Aggiorna le dimensioni del contenitore
        tracksContainer.setBounds(0, 0, tracksViewport.getMaximumVisibleWidth(), 
                                  std::max(tracksViewport.getHeight(), yPos));
    }
    
    std::unique_ptr<ModernLookAndFeel> lookAndFeel;
    AudioEngine audioEngine;
    
    // Sidebar
    SidebarComponent sidebar;
    bool sidebarCollapsed = false;
    int sidebarWidth = 220;
    
    // Pannello trasporto
    TransportPanel transportPanel{audioEngine};
    
    // Contenitore tracce
    juce::Viewport tracksViewport;
    juce::Component tracksContainer;
    juce::OwnedArray<TrackComponent> tracks;
    
    // Pulsante aggiungi traccia
    juce::TextButton addTrackButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};