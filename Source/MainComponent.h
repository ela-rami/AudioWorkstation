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
    MainComponent() : transportPanel(audioEngine)
    {
        lookAndFeel = std::make_unique<ModernLookAndFeel>();
        juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get());
        audioEngine.addListener(this);
        sidebar.addListener(this);
        addAndMakeVisible(sidebar);
        addAndMakeVisible(transportPanel);
        tracksViewport.setViewedComponent(&tracksContainer, false);
        tracksViewport.setScrollBarsShown(true, true);
        addAndMakeVisible(tracksViewport);
        tracksContainer.setPaintingIsUnclipped(true);
        addTrackButton.setButtonText("+ Add Track");
        addTrackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x20FFFFFF));
        addTrackButton.onClick = [this] { addNewTrack(); };
        addAndMakeVisible(addTrackButton);
        setSize(1600, 900);
    }

    ~MainComponent() override
    {
        audioEngine.removeListener(this);
        sidebar.removeListener(this);
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        juce::Colour color1 = juce::Colour(0xff10101b);
        juce::Colour color2 = juce::Colour(0xff2a2a3a);
        juce::Colour color3 = juce::Colour(0xff1a1a2e);
        juce::ColourGradient gradient = juce::ColourGradient::vertical(color1, 0.0f, color3, (float)getHeight());
        gradient.addColour(0.5, color2);
        g.setGradientFill(gradient);
        g.fillAll();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        sidebar.setBounds(bounds.removeFromLeft(sidebarWidth));
        auto mainArea = bounds;
        transportPanel.setBounds(mainArea.removeFromTop(70));
        auto addTrackButtonArea = mainArea.removeFromBottom(60);
        addTrackButton.setBounds(addTrackButtonArea.withSizeKeepingCentre(180, 40));
        tracksViewport.setBounds(mainArea.reduced(10));
        updateTracksLayout();
    }

    bool isInterestedInFileDrag(const juce::StringArray& files) override
    {
        for (auto& file : files)
        {
            juce::File f(file);
            if (f.hasFileExtension(".wav") || f.hasFileExtension(".mp3") ||
                f.hasFileExtension(".aiff") || f.hasFileExtension(".aif") ||
                f.hasFileExtension(".ogg") || f.hasFileExtension(".flac"))
                return true;
        }
        return false;
    }

    void filesDropped(const juce::StringArray& files, int x, int y) override
    {
        // CORREZIONE: Specifica Point<int> per risolvere ambiguità
        auto tracksContainerPos = tracksContainer.getLocalPoint(this, juce::Point<int>(x, y));

        TrackComponent* targetTrack = nullptr;
        for (auto* track : tracks)
        {
            if (track->getBoundsInParent().contains(tracksContainerPos))
            {
                targetTrack = track;
                break;
            }
        }

        if (targetTrack == nullptr)
        {
             juce::Logger::writeToLog("File dropped outside a track, creating a new one.");
             targetTrack = addNewTrack();
             if (targetTrack)
                 tracksViewport.setViewPosition(0, targetTrack->getY());
        }

        if (targetTrack != nullptr && !files.isEmpty())
        {
            juce::File file(files[0]);
            if (file.existsAsFile() && isInterestedInFileDrag({file.getFullPathName()}))
            {
                juce::Logger::writeToLog("Loading dropped file '" + file.getFileName() + "' into track " + juce::String(targetTrack->getTrackNumber()));
                targetTrack->loadFile(file);
                audioEngine.loadFile(file, targetTrack->getTrackNumber());
            }
            else {
                 juce::Logger::writeToLog("Dropped file is not a valid audio file or doesn't exist.");
            }
        }
    }

    // --- Callback da AudioEngine::Listener ---
    void fileLoaded(const juce::File& file, int trackId) override
    {
        juce::Logger::writeToLog("MainComponent notified: File loaded for track " + juce::String(trackId));
    }

    void bpmChanged(int newBpm) override {
        juce::Logger::writeToLog("MainComponent notified: BPM changed to " + juce::String(newBpm));
    }

    void keyChanged(const juce::String& newKey) override {
         juce::Logger::writeToLog("MainComponent notified: Key changed to " + newKey);
    }


    // --- Callback da TrackComponent::Listener ---
    void trackRemovalRequested(TrackComponent* trackToRemove) override
    {
        if (trackToRemove == nullptr) return;

        int trackIdToRemove = trackToRemove->getTrackNumber();
        juce::Logger::writeToLog("MainComponent: Requesting removal of track ID " + juce::String(trackIdToRemove));

        audioEngine.removeTrackAudio(trackIdToRemove);

        int trackIndex = tracks.indexOf(trackToRemove);
        if (trackIndex >= 0)
        {
             juce::Logger::writeToLog("MainComponent: Found track at index " + juce::String(trackIndex) + ". Removing from UI and list.");
             tracksContainer.removeChildComponent(tracks[trackIndex]);
             tracks.remove(trackIndex, true); // true = delete the object
             updateTracksLayout();
             juce::Logger::writeToLog("MainComponent: Track removal complete.");
        }
        else
        {
             juce::Logger::writeToLog("MainComponent Error: Track to remove not found in the list!");
             if (trackToRemove->getParentComponent() == &tracksContainer)
                 tracksContainer.removeChildComponent(trackToRemove);
        }
    }

    // --- Callback da SidebarComponent::Listener ---
    void sidebarToggleRequested(bool isNowCollapsed) override
    {
        sidebarWidth = isNowCollapsed ? 50 : 220;
        resized();
    }

private:
    TrackComponent* addNewTrack()
    {
        static int nextTrackId = 1;
        const int newTrackId = nextTrackId++;

        juce::Logger::writeToLog("MainComponent: Adding new track with ID " + juce::String(newTrackId));

        auto* track = new TrackComponent(newTrackId, audioEngine);
        track->addListener(this);
        tracks.add(track);
        tracksContainer.addAndMakeVisible(track);
        updateTracksLayout();
        tracksViewport.setViewPosition(0, tracksContainer.getHeight() - track->getHeight() - 10);

        return track;
    }

    void updateTracksLayout()
    {
        const int trackHeight = 140;
        const int trackSpacing = 10;
        const int startX = 10;
        const int startY = 10;
        const int availableWidth = tracksViewport.getMaximumVisibleWidth() - startX - (tracksViewport.isVerticalScrollBarShown() ? tracksViewport.getScrollBarThickness() : 0);

        int currentY = startY;
        for (auto* track : tracks)
        {
            track->setBounds(startX, currentY, availableWidth, trackHeight);
            currentY += trackHeight + trackSpacing;
        }

        int totalHeight = std::max(tracksViewport.getHeight(), currentY);
        int totalWidth = availableWidth + startX;
        tracksContainer.setSize(totalWidth, totalHeight);
    }

    std::unique_ptr<ModernLookAndFeel> lookAndFeel;
    AudioEngine audioEngine;

    SidebarComponent sidebar;
    TransportPanel transportPanel;
    juce::Viewport tracksViewport;
    juce::Component tracksContainer;
    juce::TextButton addTrackButton;

    juce::OwnedArray<TrackComponent> tracks;

    int sidebarWidth = 220;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};