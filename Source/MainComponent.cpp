#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(juce::Font(juce::FontOptions().withHeight(16.0f)));
    g.setColour(juce::Colours::white);
    g.drawText("Audio Workstation", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // Aggiornare il layout quando la finestra cambia dimensione
}