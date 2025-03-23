#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"

class FileLoaderComponent : public juce::Component,
                           public juce::FileDragAndDropTarget
{
public:
    FileLoaderComponent(AudioEngine& audioEngineToUse);
    ~FileLoaderComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void loadFile();

    private:
    AudioEngine& audioEngine;
    juce::TextButton loadButton;
    juce::Label fileNameLabel;
    juce::File currentFile;
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileLoaderComponent)
};