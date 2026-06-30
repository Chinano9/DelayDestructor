/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OsarioDelayDestroyerAudioProcessorEditor::OsarioDelayDestroyerAudioProcessorEditor(OsarioDelayDestroyerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLookAndFeel);

    // Función auxiliar para configurar cada fader
    auto setupFader = [this](juce::Slider& slider, juce::Label& label)
        {
            slider.setSliderStyle(juce::Slider::LinearVertical);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
            slider.setTextValueSuffix(""); 
            slider.setNumDecimalPlacesToDisplay(2);
            addAndMakeVisible(slider);

           
            label.setFont(juce::Font(juce::FontOptions(16.0f).withStyle("Bold")));
            label.setJustificationType(juce::Justification::centred);
            addAndMakeVisible(label);
        };

    setupFader(bitDepthSlider, bitDepthLabel);
    setupFader(delayTimeSlider, delayTimeLabel);
    // Esto es distinto para poder marcar el peligro del feedback a tope
    setupFader(feedbackSlider, feedbackLabel);
    feedbackSlider.setComponentID("feedback");

    // Configuración de la etiqueta de advertencia
    dangerLabel.setText("AUTO-OSCILACION", juce::dontSendNotification);
    dangerLabel.setFont(juce::Font(juce::FontOptions(13.0f).withStyle("Bold")));
    dangerLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff3333)); // Rojo brillante
    dangerLabel.setJustificationType(juce::Justification::centred);
    dangerLabel.setVisible(false); // Oculto al inicio
    addAndMakeVisible(dangerLabel);

    // ESCUCHADOR DE EVENTOS (onValueChange)
    feedbackSlider.onValueChange = [this] {
        if (feedbackSlider.getValue() >= 0.95f)
        {
            dangerLabel.setVisible(true);
        }
        else
        {
            dangerLabel.setVisible(false);
        }
        };

    setupFader(mixSlider, mixLabel);
    setupFader(harshnessSlider, harshnessLabel);

    cutoffComboBox.addItemList({ "Plano (Todo)", "Medios (0.4)", "Agudos (0.75)" }, 1);
    addAndMakeVisible(cutoffComboBox);
    cutoffLabel.setFont(juce::Font(14.0f));
    addAndMakeVisible(cutoffLabel);

    
    delayTimeAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DELAYTIME", delayTimeSlider);
    feedbackAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FEEDBACK", feedbackSlider);
    mixAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "MIX", mixSlider);
    harshnessAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "HARSHNESS", harshnessSlider);
    bitDepthAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "BITDEPTH", bitDepthSlider);
    cutoffAttach = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "CUTOFF", cutoffComboBox);

    destructionGroup.setText("DESTRUCCION");
    destructionGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(destructionGroup);

    setSize(700, 450);
}

OsarioDelayDestroyerAudioProcessorEditor::~OsarioDelayDestroyerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void OsarioDelayDestroyerAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bgGradient(juce::Colour(0xff2a2a2a), 0.0f, 0.0f,
        juce::Colour(0xff121212), 0.0f, (float)getHeight(), false);
    g.setGradientFill(bgGradient);
    g.fillRect(getLocalBounds());
}

void OsarioDelayDestroyerAudioProcessorEditor::resized()
{
    // Margen global
    auto area = getLocalBounds().reduced(20);

    // Dividimos la pantalla en dos grandes bloques:
    auto rightPanel = area.removeFromRight(280);
    area.removeFromRight(20); 
    auto leftPanel = area;

    // --- DIBUJANDO EL LADO IZQUIERDO (Delay, Feedback, Mix y Alarma) ---

    auto dangerArea = leftPanel.removeFromBottom(25);
    dangerLabel.setBounds(dangerArea);

    leftPanel.removeFromBottom(10);

    int leftChannelWidth = leftPanel.getWidth() / 3;
    int spacing = 15;

    auto placeFader = [&](juce::Slider& slider, juce::Label& label, juce::Rectangle<int>& targetArea, int width)
        {
            auto channelArea = targetArea.removeFromLeft(width);
            channelArea.removeFromRight(spacing); 
            label.setBounds(channelArea.removeFromTop(25));
            slider.setBounds(channelArea);
        };

    placeFader(delayTimeSlider, delayTimeLabel, leftPanel, leftChannelWidth);
    placeFader(feedbackSlider, feedbackLabel, leftPanel, leftChannelWidth);
    placeFader(mixSlider, mixLabel, leftPanel, leftChannelWidth);


    // --- DIBUJANDO EL LADO DERECHO (Caja de Destrucción) ---

    destructionGroup.setBounds(rightPanel);

    auto groupInnerArea = rightPanel.reduced(15);
    groupInnerArea.removeFromTop(20);

    auto comboArea = groupInnerArea.removeFromBottom(50);
    cutoffLabel.setBounds(comboArea.removeFromTop(20));
    cutoffComboBox.setBounds(comboArea);

    groupInnerArea.removeFromBottom(10);

    int rightChannelWidth = groupInnerArea.getWidth() / 2;

    placeFader(harshnessSlider, harshnessLabel, groupInnerArea, rightChannelWidth);
    placeFader(bitDepthSlider, bitDepthLabel, groupInnerArea, rightChannelWidth);
}