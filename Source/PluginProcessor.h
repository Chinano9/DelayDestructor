/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
*/
class OsarioDelayDestroyerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OsarioDelayDestroyerAudioProcessor();
    ~OsarioDelayDestroyerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Funciones de utilidades
    float readFromDelay(int channel, float delayTimeInSecs, int currentIndex);
    float processSpectralCrush(int channel, float wetSample, float destructionFactor, float harshness, float cutoffValue);
    void executeFFTMutilation(int channel, float destructionFactor, float harshness, float cutoffValue);


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OsarioDelayDestroyerAudioProcessor)

    juce::AudioProcessorValueTreeState apvts;

    // --- CONSTANTES DE ARQUITECTURA ---
    static constexpr int fftOrder = 9;
    static constexpr int fftSize = 1 << fftOrder;  // 2^9 = 512 muestras
    static constexpr int fifoSize = fftSize * 2;    // 1024 (el doble para evitar desbordamientos)

    //chingaderas del delay
    juce::AudioSampleBuffer delayBuffer;
    int writeIndex = 0;

    // --- MOTORES FFT ---
    // Orden 9 = 512 muestras (un balance decente entre latencia y resolución de frecuencia)
    juce::dsp::FFT forwardFFT{ fftOrder };
    juce::dsp::FFT inverseFFT{ fftOrder };

    // Arreglos de memoria estática para hacer los cálculos sin alojar memoria en tiempo real
    std::array<std::array<float, fifoSize>, 2> fifo = {};
    std::array<std::array<float, fifoSize>, 2> fftData = {};
    std::array<std::array<float, fifoSize>, 2> outputFifo = {};
    std::array<int, 2> fifoIndex = { 0, 0 };
    std::array<int, 2> outputFifoIndex = { 0, 0 };
    bool nextFFTBlockReady = false;         // Bandera para saber cuándo triturar

    // Función para crear los parámetros
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
};
