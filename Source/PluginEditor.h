/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MinimalStyleLookAndFeel.h"
#include "OsarioUI.h"



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
