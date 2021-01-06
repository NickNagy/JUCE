/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
// TODO: create a lib of all my files
#include "../../../MyJUCEFiles/Attachments.h"
#include "../../../MyJUCEFiles/RotarySliders.h"

//==============================================================================
/**
*/
class AnalogFiltersAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AnalogFiltersAudioProcessorEditor (AnalogFiltersAudioProcessor&, juce::AudioParameterFloat * cutoffFreq);
    ~AnalogFiltersAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AnalogFiltersAudioProcessor& audioProcessor;

    int sliderLabelHeight = 50;

    magna::RotarySlider cutoffFreqSlider;
    magna::RotarySliderParameterAttachment cutoffFreqSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogFiltersAudioProcessorEditor)
};
