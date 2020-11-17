/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpectogramToolAudioProcessorEditor  : public juce::AudioProcessorEditor
                                          , private juce::Timer
{
public:
    SpectogramToolAudioProcessorEditor (SpectogramToolAudioProcessor&);
    ~SpectogramToolAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    void drawSpectrogram(juce::Graphics&);
    void drawAnalyser(juce::Graphics&);
    void setLevels();

private:
    SpectogramToolAudioProcessor& audioProcessor;

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectogramToolAudioProcessorEditor)
};
