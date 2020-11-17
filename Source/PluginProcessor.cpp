/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectogramToolAudioProcessor::SpectogramToolAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SpectogramToolAudioProcessor::~SpectogramToolAudioProcessor()
{
}

//==============================================================================
const juce::String SpectogramToolAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpectogramToolAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpectogramToolAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpectogramToolAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpectogramToolAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpectogramToolAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpectogramToolAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpectogramToolAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpectogramToolAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpectogramToolAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpectogramToolAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void SpectogramToolAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpectogramToolAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void SpectogramToolAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (totalNumOutputChannels > 0)
    {
        auto* channelData = buffer.getReadPointer (0);
 
        for (auto i = 0; i < buffer.getNumSamples(); ++i)
            pushNextSampleIntoFifo (channelData[i]);
    }
}

//==============================================================================
bool SpectogramToolAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpectogramToolAudioProcessor::createEditor()
{
    return new SpectogramToolAudioProcessorEditor (*this);
}

//==============================================================================
void SpectogramToolAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SpectogramToolAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectogramToolAudioProcessor();
}

void SpectogramToolAudioProcessor::pushNextSampleIntoFifo (float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (! nextFFTBlockReady)
        {
            juce::zeromem (fftData, sizeof (fftData));
            memcpy (fftData, fifo, sizeof (fifo));
            nextFFTBlockReady = true;
        }
 
        fifoIndex = 0;
    }
 
    fifo[fifoIndex++] = sample;
}
