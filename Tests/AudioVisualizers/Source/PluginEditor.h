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
class AudioVisualisersAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AudioVisualisersAudioProcessorEditor (AudioVisualisersAudioProcessor&);
    ~AudioVisualisersAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioVisualisersAudioProcessor& audioProcessor;
    AudioVisualiser visualiser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioVisualisersAudioProcessorEditor)
};
