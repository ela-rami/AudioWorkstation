#pragma once

#include <JuceHeader.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        // Impostazioni colori base
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1A1A2E));
        setColour(juce::DocumentWindow::textColourId, juce::Colours::white);
        
        // Pulsanti
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252537));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4EE6B8));
        setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        
        // Slider
        setColour(juce::Slider::backgroundColourId, juce::Colour(0x20FFFFFF));
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colour(0xff4EE6B8));
        
        // Labels
        setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, 
                             bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto cornerSize = 15.0f;
        
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);
        
        if (shouldDrawButtonAsDown || button.getToggleState())
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.1f);
            
        if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);
            
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
    }
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearHorizontal)
        {
            const float cornerSize = 5.0f;
            
            // Disegna lo sfondo dello slider
            g.setColour(slider.findColour(juce::Slider::backgroundColourId));
            g.fillRoundedRectangle(x, y + height * 0.35f, width, height * 0.3f, cornerSize);
            
            // Disegna la traccia valorizzata
            const auto valueWidth = sliderPos - x;
            if (valueWidth > 0)
            {
                g.setColour(slider.findColour(juce::Slider::trackColourId));
                g.fillRoundedRectangle(x, y + height * 0.35f, valueWidth, height * 0.3f, cornerSize);
            }
            
            // Disegna il thumb (manopola)
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.fillEllipse(sliderPos - 6, y + height * 0.5f - 6, 12, 12);
        }
        else
        {
            // Per altri stili, usa l'implementazione predefinita
            juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }
    
    // Personalizzazione di altri elementi della UI può essere aggiunta qui
};