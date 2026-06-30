#pragma once

namespace OsarioUI
{

    namespace Colours
    {
        // Fondo general (Gradiente)
        constexpr juce::uint32 bgTop = 0xff2a2a2a;
        constexpr juce::uint32 bgBottom = 0xff121212;

        // Cajas y Texto
        constexpr juce::uint32 groupOutline = 0xff4a4a4a;
        constexpr juce::uint32 textWhite = 0xffffffff;
        constexpr juce::uint32 dangerText = 0xffff3333; // Rojo brillante para texto

        // Sliders (Fijos)
        constexpr juce::uint32 trackBg = 0xff1e1e1e;
        constexpr juce::uint32 trackEmptyTop = 0xff0a0a0a;
        constexpr juce::uint32 trackEmptyBot = 0xff2a2a2a;

        constexpr juce::uint32 thumbTop = 0xffffffff;
        constexpr juce::uint32 thumbBot = 0xffbbbbbb;

        // Sliders (Gradientes Dinámicos)
        constexpr juce::uint32 synthBlue = 0xff3f5efb; // Arriba
        constexpr juce::uint32 synthPink = 0xfffc466b; // Abajo

        constexpr juce::uint32 dangerRedTop = 0xffff2222; // Arriba (Neón)
        constexpr juce::uint32 dangerRedBot = 0xff550000; // Abajo (Sangre)

        // ComboBox
        constexpr juce::uint32 comboBg = 0xff2d2d2d;
    }

    // --- LÓGICA Y LAYOUT ---
    constexpr float feedbackDangerThreshold = 0.85f;

    // Dimensiones de los faders
    constexpr float faderTrackWidth = 8.0f;
    constexpr float faderThumbWidth = 32.0f;
    constexpr float faderThumbHeight = 12.0f;

    // Dimensiones de la ventana
    constexpr int windowWidth = 700;
    constexpr int windowHeight = 450;

    // CONSTANTES DE LAYOUT 
    constexpr int globalMargin = 20;
    constexpr int panelGap = 20;
    constexpr int rightPanelWidth = 280;

    constexpr int labelHeight = 25;
    constexpr int faderSpacing = 15;
    constexpr int verticalMargin = 10;

    constexpr int groupInnerMargin = 15;
    constexpr int groupTitleOffset = 20;

    constexpr int comboAreaHeight = 50;
    constexpr int comboLabelHeight = 20;
}