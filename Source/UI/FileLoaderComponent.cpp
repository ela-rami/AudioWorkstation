#include "FileLoaderComponent.h"

FileLoaderComponent::FileLoaderComponent(AudioEngine& audioEngineToUse)
    : audioEngine(audioEngineToUse)
{
    // Configure the file load button
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load Audio File");
    loadButton.onClick = [this] { loadFile(); };

    // Configure the file name label
    addAndMakeVisible(fileNameLabel);
    fileNameLabel.setText("Drag & drop audio files here or click 'Load'", juce::dontSendNotification);
    fileNameLabel.setJustificationType(juce::Justification::centred);
}

FileLoaderComponent::~FileLoaderComponent()
{
}

void FileLoaderComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawRect(getLocalBounds(), 1);
}

void FileLoaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    loadButton.setBounds(bounds.removeFromBottom(30));
    bounds.removeFromBottom(5);
    fileNameLabel.setBounds(bounds);
}

bool FileLoaderComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Accept only audio files
    for (auto& file : files)
    {
        if (file.endsWithIgnoreCase(".wav") || 
            file.endsWithIgnoreCase(".mp3") || 
            file.endsWithIgnoreCase(".aiff") || 
            file.endsWithIgnoreCase(".aif") || 
            file.endsWithIgnoreCase(".ogg"))
            return true;
    }
    
    return false;
}

void FileLoaderComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (const auto& filePath : files)
    {
        currentFile = juce::File(filePath);
        if (audioEngine.loadFile(currentFile))
        {
            fileNameLabel.setText(currentFile.getFileName(), juce::dontSendNotification);
            return;
        }
    }
}

void FileLoaderComponent::loadFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file to load...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav;*.mp3;*.aiff;*.aif;*.ogg");
    
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto result = fc.getResult();
            if (result.existsAsFile())
            {
                currentFile = result;
                if (audioEngine.loadFile(currentFile))
                {
                    fileNameLabel.setText(currentFile.getFileName(), juce::dontSendNotification);
                }
            }
        });
}