#pragma once

#include <JuceHeader.h>

class SidebarComponent : public juce::Component
{
public:
    SidebarComponent()
    {
        // Titolo browser
        browserTitleLabel.setText("BROWSER", juce::dontSendNotification);
        browserTitleLabel.setFont(juce::Font().withStyle(juce::Font::bold).withHeight(16.0f));
        browserTitleLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        browserTitleLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(browserTitleLabel);
        
        // Pulsante toggle
        toggleButton.setButtonText("<");
        toggleButton.onClick = [this] { toggleSidebar(); };
        addAndMakeVisible(toggleButton);
        
        // Categorie
        setupCategory(filesButton, "Files");
        setupCategory(favoritesButton, "Favorites");
        setupCategory(recentButton, "Recent");
        
        // Cartelle (mockup)
        addFolderButton("Drum Loops");
        addFolderButton("Bass Lines");
        addFolderButton("Melodies");
        addFolderButton("Vocals");
        addFolderButton("FX");
        addFolderButton("Projects");
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff161626));
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        
        // Titolo e pulsante toggle
        auto headerArea = bounds.removeFromTop(30);
        browserTitleLabel.setBounds(headerArea.removeFromLeft(100));
        toggleButton.setBounds(headerArea.removeFromRight(30));
        
        bounds.removeFromTop(10);
        
        // Categorie
        filesButton.setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(10);
        favoritesButton.setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(10);
        recentButton.setBounds(bounds.removeFromTop(30));
        bounds.removeFromTop(20);
        
        // Cartelle
        auto foldersBounds = bounds.removeFromTop(270);
        for (auto* button : folderButtons)
        {
            button->setBounds(foldersBounds.removeFromTop(30));
            foldersBounds.removeFromTop(10);
        }
    }
    
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void sidebarToggleRequested() = 0;
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
    void setupCategory(juce::TextButton& button, const juce::String& text)
    {
        button.setButtonText(text);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252537));
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        addAndMakeVisible(button);
    }
    
    void addFolderButton(const juce::String& name)
    {
        auto* button = new juce::TextButton();
        button->setButtonText("📁 " + name);
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252537));
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        addAndMakeVisible(button);
        
        folderButtons.add(button);
    }
    
    void toggleSidebar()
    {
        listeners.call(&Listener::sidebarToggleRequested);
    }
    
    juce::Label browserTitleLabel;
    juce::TextButton toggleButton;
    
    juce::TextButton filesButton;
    juce::TextButton favoritesButton;
    juce::TextButton recentButton;
    
    juce::OwnedArray<juce::TextButton> folderButtons;
    juce::ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};