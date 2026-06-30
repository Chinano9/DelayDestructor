/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class MinimalStyleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MinimalStyleLookAndFeel()
    {
        // Colores globales
        setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff4a4a4a)); // Borde oscuro
        setColour(juce::GroupComponent::textColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colour(0xff121212)); // Fondo riel más oscuro
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff00d2ff)); // Color principal azul

        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2d2d2d));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);

        // Configuraciones globales para los sliders
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1e1e1e));
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

        float trackWidth = 8.0f;
        auto trackRect = juce::Rectangle<float>((float)x + (width * 0.5f) - (trackWidth * 0.5f), (float)y, trackWidth, (float)height);

        // Gradiente del riel oscuro de fondo
        juce::ColourGradient trackGradient(juce::Colour(0xff0a0a0a), trackRect.getX(), trackRect.getY(),
            juce::Colour(0xff2a2a2a), trackRect.getX(), trackRect.getBottom(), false);
        g.setGradientFill(trackGradient);
        g.fillRect(trackRect);

        float filledHeight = std::max(0.0f, trackRect.getBottom() - sliderPos);

        if (filledHeight > 0.0f)
        {
            auto fillRect = trackRect.withTop(sliderPos).withHeight(filledHeight);
            juce::ColourGradient fillGradient;

            // Comprobamos si el componente es el fader de feedback y cruzó el umbral de peligro
            if (slider.getComponentID() == "feedback" && slider.getValue() >= 0.95f)
            {
                // Gradiente Rojo Peligro
                fillGradient = juce::ColourGradient(
                    juce::Colour(0xffff2222), fillRect.getX(), fillRect.getY(),
                    juce::Colour(0xff550000), fillRect.getX(), fillRect.getBottom(), false);
            }
            else
            {
                // Gradiente Synthwave 
                fillGradient = juce::ColourGradient(
                    juce::Colour(0xff3f5efb), fillRect.getX(), fillRect.getY(),
                    juce::Colour(0xfffc466b), fillRect.getX(), fillRect.getBottom(), false);
            }

            g.setGradientFill(fillGradient);
            g.fillRect(fillRect);
        }

        // Agarradera (Thumb) con tamaño fijo en píxeles
        float thumbWidth = 32.0f;
        float thumbHeight = 12.0f;
        juce::Rectangle<float> thumbRect((float)x + (width * 0.5f) - (thumbWidth * 0.5f), sliderPos - (thumbHeight * 0.5f), thumbWidth, thumbHeight);

        juce::ColourGradient thumbGradient(juce::Colours::white, thumbRect.getX(), thumbRect.getY(),
            juce::Colour(0xffbbbbbb), thumbRect.getX(), thumbRect.getBottom(), false);
        g.setGradientFill(thumbGradient);
        g.fillRect(thumbRect);
    }
};
//==============================================================================
/**
*/
class OsarioDelayDestroyerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OsarioDelayDestroyerAudioProcessorEditor (OsarioDelayDestroyerAudioProcessor&);
    ~OsarioDelayDestroyerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OsarioDelayDestroyerAudioProcessor& audioProcessor;

    // Instancia de nuestro tema custom
    MinimalStyleLookAndFeel customLookAndFeel;

    // Componentes de UI
    juce::Slider bitDepthSlider;
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider mixSlider;
    juce::Slider harshnessSlider;
    juce::ComboBox cutoffComboBox;

    juce::GroupComponent destructionGroup;


    // Etiquetas
    juce::Label bitDepthLabel{ {}, "Bitcrush" };
    juce::Label delayTimeLabel{ {}, "Time" };
    juce::Label feedbackLabel{ {}, "Feedback" };

    juce::Label dangerLabel;
    juce::Label mixLabel{ {}, "Mix" };
    juce::Label harshnessLabel{ {}, "Harshness" };
    juce::Label cutoffLabel{ {}, "Cutoff Mode" };

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> bitDepthAttach;
    std::unique_ptr<SliderAttachment> delayTimeAttach;
    std::unique_ptr<SliderAttachment> feedbackAttach;
    std::unique_ptr<SliderAttachment> mixAttach;
    std::unique_ptr<SliderAttachment> harshnessAttach;
    std::unique_ptr<ComboBoxAttachment> cutoffAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OsarioDelayDestroyerAudioProcessorEditor)
};
