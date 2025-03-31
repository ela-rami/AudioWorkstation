#pragma once

#include <JuceHeader.h>

class SidebarComponent : public juce::Component
{
public:
    SidebarComponent() : isCollapsed(false)
    {
        browserTitleLabel.setFont(juce::Font(16.0f).withStyle(juce::Font::bold));
        browserTitleLabel.setText("BROWSER", juce::dontSendNotification);
        browserTitleLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        browserTitleLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(browserTitleLabel);

        toggleButton.setButtonText("<");
        toggleButton.onClick = [this] { toggleSidebar(); };
        addAndMakeVisible(toggleButton);

        setupCategory(filesButton, "Files");
        setupCategory(favoritesButton, "Favorites");
        setupCategory(recentButton, "Recent");

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
        if (isCollapsed)
        {
            toggleButton.setBounds(10, 10, 30, 30);
            browserTitleLabel.setVisible(false);
            filesButton.setVisible(false);
            favoritesButton.setVisible(false);
            recentButton.setVisible(false);
            for (auto* button : folderButtons)
                button->setVisible(false);
        }
        else
        {
            auto bounds = getLocalBounds().reduced(10);
            auto headerArea = bounds.removeFromTop(30);
            browserTitleLabel.setBounds(headerArea.removeFromLeft(100));
            toggleButton.setBounds(headerArea.removeFromRight(30));

            browserTitleLabel.setVisible(true);
            filesButton.setVisible(true);
            favoritesButton.setVisible(true);
            recentButton.setVisible(true);

            bounds.removeFromTop(10);
            filesButton.setBounds(bounds.removeFromTop(30));
            bounds.removeFromTop(10);
            favoritesButton.setBounds(bounds.removeFromTop(30));
            bounds.removeFromTop(10);
            recentButton.setBounds(bounds.removeFromTop(30));
            bounds.removeFromTop(20);

            auto foldersBounds = bounds.removeFromTop(270);
            int folderButtonHeight = 30;
            int folderSpacing = 10;
            for (auto* button : folderButtons)
            {
                button->setVisible(true);
                if (foldersBounds.getHeight() >= folderButtonHeight)
                {
                    button->setBounds(foldersBounds.removeFromTop(folderButtonHeight));
                    if (foldersBounds.getHeight() >= folderSpacing)
                        foldersBounds.removeFromTop(folderSpacing);
                } else {
                     button->setVisible(false);
                }
            }
        }
    }

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void sidebarToggleRequested(bool isNowCollapsed) = 0;
    };

    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

    void setCollapsed(bool shouldBeCollapsed)
    {
        if (isCollapsed != shouldBeCollapsed)
        {
            isCollapsed = shouldBeCollapsed;
            toggleButton.setButtonText(isCollapsed ? ">" : "<");
            resized();
            repaint();
        }
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
        button->setButtonText(name);
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252537));
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        // CORREZIONE: Rimosse chiamate non esistenti
        // button->setJustificationType(juce::Justification::centredLeft);
        // button->setTextOffsetX(10);
        addAndMakeVisible(button);
        folderButtons.add(button);
    }

    void toggleSidebar()
    {
        isCollapsed = !isCollapsed;
        toggleButton.setButtonText(isCollapsed ? ">" : "<");
        resized();
        repaint();
        listeners.call(&Listener::sidebarToggleRequested, isCollapsed);
    }

    juce::Label browserTitleLabel;
    juce::TextButton toggleButton;

    juce::TextButton filesButton;
    juce::TextButton favoritesButton;
    juce::TextButton recentButton;

    juce::OwnedArray<juce::TextButton> folderButtons;
    juce::ListenerList<Listener> listeners;

    bool isCollapsed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};