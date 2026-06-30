#pragma once
#include <JuceHeader.h>
#include "OsarioUI.h"

class MinimalStyleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MinimalStyleLookAndFeel()
    {
        // Colores globales leídos desde nuestro namespace
        setColour(juce::GroupComponent::outlineColourId, juce::Colour(OsarioUI::Colours::groupOutline));
        setColour(juce::GroupComponent::textColourId, juce::Colour(OsarioUI::Colours::textWhite));

        setColour(juce::ComboBox::backgroundColourId, juce::Colour(OsarioUI::Colours::comboBg));
        setColour(juce::ComboBox::textColourId, juce::Colour(OsarioUI::Colours::textWhite));

        setColour(juce::Slider::backgroundColourId, juce::Colour(OsarioUI::Colours::trackBg));
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style != juce::Slider::LinearVertical)
        {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
            return;
        }

        auto trackRect = juce::Rectangle<float>((float)x + (width * 0.5f) - (OsarioUI::faderTrackWidth * 0.5f),
            (float)y, OsarioUI::faderTrackWidth, (float)height);

        // Riel Oscuro
        juce::ColourGradient trackGradient(
            juce::Colour(OsarioUI::Colours::trackEmptyTop), trackRect.getX(), trackRect.getY(),
            juce::Colour(OsarioUI::Colours::trackEmptyBot), trackRect.getX(), trackRect.getBottom(), false);
        g.setGradientFill(trackGradient);
        g.fillRect(trackRect);

        float filledHeight = std::max(0.0f, trackRect.getBottom() - sliderPos);

        if (filledHeight > 0.0f)
        {
            auto fillRect = trackRect.withTop(sliderPos).withHeight(filledHeight);
            juce::ColourGradient fillGradient;

            if (slider.getComponentID() == "feedback" && slider.getValue() >= OsarioUI::feedbackDangerThreshold)
            {
                // Gradiente Rojo Peligro
                fillGradient = juce::ColourGradient(
                    juce::Colour(OsarioUI::Colours::dangerRedTop), fillRect.getX(), fillRect.getY(),
                    juce::Colour(OsarioUI::Colours::dangerRedBot), fillRect.getX(), fillRect.getBottom(), false);
            }
            else
            {
                // Gradiente Synthwave 
                fillGradient = juce::ColourGradient(
                    juce::Colour(OsarioUI::Colours::synthBlue), fillRect.getX(), fillRect.getY(),
                    juce::Colour(OsarioUI::Colours::synthPink), fillRect.getX(), fillRect.getBottom(), false);
            }

            g.setGradientFill(fillGradient);
            g.fillRect(fillRect);
        }

        // Agarradera (Thumb)
        juce::Rectangle<float> thumbRect((float)x + (width * 0.5f) - (OsarioUI::faderThumbWidth * 0.5f),
            sliderPos - (OsarioUI::faderThumbHeight * 0.5f),
            OsarioUI::faderThumbWidth, OsarioUI::faderThumbHeight);

        juce::ColourGradient thumbGradient(
            juce::Colour(OsarioUI::Colours::thumbTop), thumbRect.getX(), thumbRect.getY(),
            juce::Colour(OsarioUI::Colours::thumbBot), thumbRect.getX(), thumbRect.getBottom(), false);
        g.setGradientFill(thumbGradient);
        g.fillRect(thumbRect);
    }
};