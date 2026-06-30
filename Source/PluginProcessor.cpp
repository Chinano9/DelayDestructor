/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OsarioDelayDestroyerAudioProcessor::OsarioDelayDestroyerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    apvts.state = juce::ValueTree("PARAMETERS");

    
    delayTimePtr = apvts.getRawParameterValue("DELAYTIME");
    feedbackPtr = apvts.getRawParameterValue("FEEDBACK");
    mixPtr = apvts.getRawParameterValue("MIX");
    harshnessPtr = apvts.getRawParameterValue("HARSHNESS");
    bitDepthPtr = apvts.getRawParameterValue("BITDEPTH");
    cutoffPtr = apvts.getRawParameterValue("CUTOFF");
}

juce::AudioProcessorValueTreeState::ParameterLayout OsarioDelayDestroyerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;



    // Nombre interno, Nombre visible, Mínimo, Máximo, Valor por defecto
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DELAYTIME", "Tiempo (s)", 0.01f, 2.0f, 0.5f));        // Hasta 2 segundos
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FEEDBACK", "Retroalimentacion", 0.0f, 1.0f, 0.5f));   
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MIX", "Mezcla (Mix)", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "HARSHNESS", "Dureza", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "BITDEPTH", "Destruccion (Bits)", 1.0f, 16.0f, 3.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(//esto seria un switch de 3 posiciones
        "CUTOFF",
        "Cutoff Mode",
        juce::StringArray{ "Plano (Todo)", "Medios (0.4)", "Agudos (0.75)" }, 
        0 // El índice por defecto (0 = Plano)
    ));
    return { params.begin(), params.end() };
}

OsarioDelayDestroyerAudioProcessor::~OsarioDelayDestroyerAudioProcessor()
{
}

//==============================================================================
const juce::String OsarioDelayDestroyerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OsarioDelayDestroyerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OsarioDelayDestroyerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OsarioDelayDestroyerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OsarioDelayDestroyerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OsarioDelayDestroyerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OsarioDelayDestroyerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OsarioDelayDestroyerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OsarioDelayDestroyerAudioProcessor::getProgramName (int index)
{
    return {};
}

void OsarioDelayDestroyerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OsarioDelayDestroyerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Calculamos cuántas muestras caben en 2 segundos de audio
    int maxDelaySamples = static_cast<int>(sampleRate * 2.0);

    // Le damos tamaño al búfer (mismo número de canales de entrada, y el tamaño en muestras)
    delayBuffer.setSize(getTotalNumInputChannels(), maxDelaySamples);

    // Limpiamos la memoria para que no tenga basura acumulada
    delayBuffer.clear();

    for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        std::fill(fifo[i].begin(), fifo[i].end(), 0.0f);
        std::fill(outputFifo[i].begin(), outputFifo[i].end(), 0.0f);
        std::fill(fftData[i].begin(), fftData[i].end(), 0.0f);
        fifoIndex[i] = 0;
        outputFifoIndex[i] = 0;
    }

    // Reiniciamos el índice de escritura
    writeIndex = 0;
}

void OsarioDelayDestroyerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OsarioDelayDestroyerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

float OsarioDelayDestroyerAudioProcessor::readFromDelay(int channel, float delayTimeInSecs, int currentIndex)
{
    int delayBufferSize = delayBuffer.getNumSamples();
    int delaySamplesToDelay = static_cast<int>(delayTimeInSecs * getSampleRate());
    int readIndex = currentIndex - delaySamplesToDelay;

    if (readIndex < 0)
        readIndex += delayBufferSize;

    return delayBuffer.getSample(channel, readIndex);
}

void OsarioDelayDestroyerAudioProcessor::executeFFTMutilation(int channel, float destructionFactor, float harshness, float cutoffValue)
{
    // Ventana Senoidal de Análisis y copia al buffer
    for (int k = 0; k < fftSize; ++k)
    {
        // Difuminamos los bordes de la señal antes de transformarla
        float sineWindow = std::sin(juce::MathConstants<float>::pi * k / fftSize);
        fftData[channel][k] = fifo[channel][k] * sineWindow;
    }

    std::fill(fftData[channel].begin() + fftSize, fftData[channel].end(), 0.0f);

    forwardFFT.performRealOnlyForwardTransform(fftData[channel].data());

    float maxMagnitude = 0.0f;
    for (int i = 0; i < fftSize * 2; i += 2)
    {
        float real = fftData[channel][i];
        float imag = fftData[channel][i + 1];
        float mag = std::sqrt((real * real) + (imag * imag));
        if (mag > maxMagnitude) maxMagnitude = mag;
    }

    float dynamicThreshold = maxMagnitude * destructionFactor;
    float maxTiltMultiplier = harshness * 12.0f;

    for (int i = 0; i < fftSize * 2; i += 2)
    {
        float real = fftData[channel][i];
        float imag = fftData[channel][i + 1];
        float magnitude = std::sqrt((real * real) + (imag * imag));

        
        float progress = (float)i / (float)(fftSize * 2);
        float shiftedProgress = std::max(0.0f, progress - cutoffValue);

        float aggressiveTilt = 1.0f + std::pow(shiftedProgress, 2.0f) * maxTiltMultiplier;
        float currentThreshold = dynamicThreshold * aggressiveTilt;

        if (magnitude < currentThreshold)
        {
            // Frecuencia asesinada
            fftData[channel][i] = 0.0f;
            fftData[channel][i + 1] = 0.0f;
        }
        else if (magnitude > 0.0001f)
        {
            // Frecuencia sobreviviente: Le aplicamos Cuantización Espectral (El Bitcrush)
            float phase = std::atan2(imag, real);

            // Reducimos los escalones disponibles de volumen (textura robótica/MP3 viejo)
            float steps = std::max(3.0f, 32.0f * (1.0f - destructionFactor));

            float normalizedMag = magnitude / maxMagnitude;
            float quantizedMag = std::round(normalizedMag * steps) / steps;
            float newMagnitude = quantizedMag * maxMagnitude;

            // Reconstruimos con la fase intacta pero magnitud aplastada
            fftData[channel][i] = newMagnitude * std::cos(phase);
            fftData[channel][i + 1] = newMagnitude * std::sin(phase);
        }
    }

    inverseFFT.performRealOnlyInverseTransform(fftData[channel].data());

    // Ventana Senoidal de Síntesis y Overlap-Add
    for (int k = 0; k < fftSize; ++k)
    {
        float sineWindow = std::sin(juce::MathConstants<float>::pi * k / fftSize);
        float windowedSample = fftData[channel][k] * sineWindow;

        // SUMA CIRCULAR: Agregamos el audio procesado empalmándolo con lo que ya estaba
        int circularIndex = (outputFifoIndex[channel] + k) % fftSize;
        outputFifo[channel][circularIndex] += windowedSample;
    }
}

float OsarioDelayDestroyerAudioProcessor::processSpectralCrush(int channel, float wetSample, float destructionFactor, float harshness, float cutoffValue)
{
    // Tamaño del salto (50% de superposición)
    int hopSize = fftSize / 2;

    // EXTRAER Y LIMPIAR (Fundamental para que no se acumule el volumen al infinito)
    float processedSample = outputFifo[channel][outputFifoIndex[channel]];
    outputFifo[channel][outputFifoIndex[channel]] = 0.0f;

    outputFifoIndex[channel]++;
    if (outputFifoIndex[channel] >= fftSize) {
        outputFifoIndex[channel] = 0;
    }

    // ALIMENTAR EL EMBUDO
    fifo[channel][fifoIndex[channel]] = wetSample;
    fifoIndex[channel]++;

    // EJECUTAR AL 50% DE LLENADO
    if (fifoIndex[channel] >= fftSize)
    {
        executeFFTMutilation(channel, destructionFactor, harshness, cutoffValue);

        std::copy(fifo[channel].begin() + hopSize, fifo[channel].end(), fifo[channel].begin());
        fifoIndex[channel] = hopSize;
    }

    return processedSample;
}

void OsarioDelayDestroyerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());


    float bitDepth = bitDepthPtr->load();
    float delayTime = delayTimePtr->load();
    float feedback = feedbackPtr->load();
    float mix = mixPtr->load();
    float harshness = harshnessPtr->load();
    int cutoffIndex = static_cast<int>(cutoffPtr->load());


    float cutoffValue = 0.0f;
    if (cutoffIndex == 1) cutoffValue = 0.4f;
    if (cutoffIndex == 2) cutoffValue = 0.75f;

    // Pre-calcular el factor de destrucción para ahorrar CPU
    float normValue = (16.0f - bitDepth) / 15.0f; // Va de 0.0 a 1.0
    float destructionFactor = std::pow(normValue, 2.0f) * 0.02f;
    int delayBufferSize = delayBuffer.getNumSamples();
    int localWriteIndex = 0;

    // Red de seguridad para no reventar timpanos
    float internalDrive = 1.2f;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(channel);

        localWriteIndex = writeIndex;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float cleanSample = channelData[sample];

            // Leer el pasado
            float wetSample = readFromDelay(channel, delayTime, localWriteIndex);

            // Destruir (FFT MP3 Effect) 
            float processedSample = processSpectralCrush(channel, wetSample, destructionFactor, harshness, cutoffValue);

            float rawFeedback = cleanSample + (processedSample * feedback);
            
            // Neta evitemos reventar tweeters jej
            float safeFeedback = std::tanh(rawFeedback * internalDrive);

            // Escribir al futuro (Feedback)
            delayData[localWriteIndex] = safeFeedback;

            localWriteIndex++;
            if (localWriteIndex >= delayBufferSize)
                localWriteIndex = 0;

            // Mezcla final
            channelData[sample] = (cleanSample * (1.0f - mix)) + (processedSample * mix);
        }
    }

    writeIndex = localWriteIndex;
}


//==============================================================================
bool OsarioDelayDestroyerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OsarioDelayDestroyerAudioProcessor::createEditor()
{
    return new OsarioDelayDestroyerAudioProcessorEditor(*this);
}

//==============================================================================
void OsarioDelayDestroyerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
}

void OsarioDelayDestroyerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
       
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OsarioDelayDestroyerAudioProcessor();
}
