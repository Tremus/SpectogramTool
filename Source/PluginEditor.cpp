/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectogramToolAudioProcessorEditor::SpectogramToolAudioProcessorEditor (SpectogramToolAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , forwardFFT (p.fftOrder)
    , window (p.fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris)
{
    setOpaque (true);
    startTimerHz (120);
    setSize (700, 500);
}

SpectogramToolAudioProcessorEditor::~SpectogramToolAudioProcessorEditor()
{
}

//==============================================================================
void SpectogramToolAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setOpacity (1.0f);
    drawSpectrogram(g);
    //drawAnalyser(g);

    audioProcessor.nextFFTBlockReady = false;
}

void SpectogramToolAudioProcessorEditor::resized()
{
}

void SpectogramToolAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.nextFFTBlockReady)
    {
        setLevels();
        audioProcessor.nextFFTBlockReady = false;
        repaint();
    }
}


void SpectogramToolAudioProcessorEditor::drawSpectrogram(juce::Graphics& g)
{
    auto width  = getWidth();
    auto height = getHeight();

    for (int x = 1; x < width; ++x)
    {
        float proportion = std::exp (std::log ((float) x / (float) width) * 2.0f);
        float pos = juce::jmap<float>(proportion, 0, audioProcessor.scopeSize - 1);
        int idx1 = pos;
        int idx2 = idx1 + 2;
        float diff = pos - idx1;

        float sample1 = audioProcessor.scopeData[idx1];
        float sample2 = audioProcessor.scopeData[idx2];
        float level = juce::jmap<float>(diff, sample1, sample2);

        // yellow 0.17
        // blue   0.65
        float hue = juce::jmap<float>(level, 0.0, 1.0, -0.35, 0.12);
        if (hue < 0.0)
            hue += 1.0;

        auto colour = juce::Colour::fromHSV (hue, 1.0f, level, 1.0);
        g.setColour(colour);
        g.drawVerticalLine(x, 0, height);
    }
}

void SpectogramToolAudioProcessorEditor::drawAnalyser(juce::Graphics& g)
{
    auto width  = getWidth();
    auto height = getHeight();

    g.setColour (juce::Colours::white);
    for (int i = 1; i < audioProcessor.scopeSize; ++i)
    {
 
        g.drawLine ({ (float) juce::jmap<float>(i - 1, 0, audioProcessor.scopeSize - 1, 0, width),
                              juce::jmap<float>(audioProcessor.scopeData[i - 1], 0.0f, 1.0f, (float) height, 0.0f),
                      (float) juce::jmap<float>(i,     0, audioProcessor.scopeSize - 1, 0, width),
                              juce::jmap<float>(audioProcessor.scopeData[i],     0.0f, 1.0f, (float) height, 0.0f) });
    }
}

void SpectogramToolAudioProcessorEditor::setLevels()
{
    window.multiplyWithWindowingTable (audioProcessor.fftData, audioProcessor.fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform (audioProcessor.fftData);
 
    auto mindB = -100.0f;
    auto maxdB =    0.0f;
 
    for (int i = 0; i < audioProcessor.scopeSize; ++i)
    {
        float skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float) i / (float) audioProcessor.scopeSize) * 0.2f);
        int fftDataIndex = juce::jlimit (0, audioProcessor.fftSize / 2, (int) (skewedProportionX * (float) audioProcessor.fftSize * 0.5f));

        // interpolation
        float pos = juce::jlimit<float>(0, audioProcessor.fftSize / 2, (skewedProportionX * (float) audioProcessor.fftSize * 0.5f));
        int idx1 = pos;
        int idx2 = idx1 + 1;
        float diff = pos - idx1;
        float val = juce::jmap<float>(diff, audioProcessor.fftData[idx1], audioProcessor.fftData[idx2]);

        float db = juce::Decibels::gainToDecibels (val) - juce::Decibels::gainToDecibels ((float) audioProcessor.fftSize);
        float l = juce::jlimit<float>(mindB, maxdB, db);
        float level = juce::jmap<float>(l, mindB, maxdB, 0.0f, 1.0f);
 
        audioProcessor.scopeData[i] = level;
    }
}
