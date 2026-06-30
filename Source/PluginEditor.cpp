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
    dangerLabel.setColour(juce::Label::textColourId, juce::Colour(OsarioUI::Colours::dangerText)); // Rojo brillante
    dangerLabel.setJustificationType(juce::Justification::centred);
    dangerLabel.setVisible(false); // Oculto al inicio
    addAndMakeVisible(dangerLabel);

    // ESCUCHADOR DE EVENTOS (onValueChange)
    feedbackSlider.onValueChange = [this] {
        if (feedbackSlider.getValue() >= OsarioUI::feedbackDangerThreshold)
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

    setSize(OsarioUI::windowWidth, OsarioUI::windowHeight);
}

OsarioDelayDestroyerAudioProcessorEditor::~OsarioDelayDestroyerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void OsarioDelayDestroyerAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bgGradient(
        juce::Colour(OsarioUI::Colours::bgTop), 0.0f, 0.0f,
        juce::Colour(OsarioUI::Colours::bgBottom), 0.0f, (float)getHeight(), false);
    g.setGradientFill(bgGradient);
    g.fillRect(getLocalBounds());
}

void OsarioDelayDestroyerAudioProcessorEditor::resized()
{
    // Margen global
    auto area = getLocalBounds().reduced(OsarioUI::globalMargin);

    // Dividimos la pantalla en dos grandes bloques:
    auto rightPanel = area.removeFromRight(OsarioUI::rightPanelWidth);
    area.removeFromRight(OsarioUI::panelGap);
    auto leftPanel = area;

    // --- DIBUJANDO EL LADO IZQUIERDO (Delay, Feedback, Mix y Alarma) ---

    // Reservamos espacio de abajo para la etiqueta de peligro
    auto dangerArea = leftPanel.removeFromBottom(OsarioUI::labelHeight);
    dangerLabel.setBounds(dangerArea);

    leftPanel.removeFromBottom(OsarioUI::verticalMargin);

    int leftChannelWidth = leftPanel.getWidth() / 3;

    // Lambda actualizada con tus constantes
    auto placeFader = [&](juce::Slider& slider, juce::Label& label, juce::Rectangle<int>& targetArea, int width)
        {
            auto channelArea = targetArea.removeFromLeft(width);
            channelArea.removeFromRight(OsarioUI::faderSpacing);

            label.setBounds(channelArea.removeFromTop(OsarioUI::labelHeight));
            slider.setBounds(channelArea);
        };

    placeFader(delayTimeSlider, delayTimeLabel, leftPanel, leftChannelWidth);
    placeFader(feedbackSlider, feedbackLabel, leftPanel, leftChannelWidth);
    placeFader(mixSlider, mixLabel, leftPanel, leftChannelWidth);


    // --- DIBUJANDO EL LADO DERECHO (Caja de Destrucción) ---

    destructionGroup.setBounds(rightPanel);

    auto groupInnerArea = rightPanel.reduced(OsarioUI::groupInnerMargin);
    groupInnerArea.removeFromTop(OsarioUI::groupTitleOffset);

    // Área del ComboBox de Cutoff
    auto comboArea = groupInnerArea.removeFromBottom(OsarioUI::comboAreaHeight);
    cutoffLabel.setBounds(comboArea.removeFromTop(OsarioUI::comboLabelHeight));
    cutoffComboBox.setBounds(comboArea);

    groupInnerArea.removeFromBottom(OsarioUI::verticalMargin);

    int rightChannelWidth = groupInnerArea.getWidth() / 2;

    placeFader(harshnessSlider, harshnessLabel, groupInnerArea, rightChannelWidth);
    placeFader(bitDepthSlider, bitDepthLabel, groupInnerArea, rightChannelWidth);
}