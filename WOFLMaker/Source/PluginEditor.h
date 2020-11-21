/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotarySliders.h"
//#include "AudioProcessorValueTreeStateExtended.h" <- don't use!

class LFOSliderListenerLabel : public juce::Slider::Listener, public juce::Label {
public:
    LFOSliderListenerLabel() {
        setText("0", juce::NotificationType::dontSendNotification);
    };
    ~LFOSliderListenerLabel() override {};
private:
    void sliderValueChanged(juce::Slider* slider) override {
        auto toBPM = slider->getValue() / 60.0f;
        setText(JUCE_TO_STRING(toBPM), juce::NotificationType::dontSendNotification);
    }
};

//==============================================================================
/**
*/
class WoflmakerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor&, juce::AudioProcessorValueTreeState& params);
    ~WoflmakerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WoflmakerAudioProcessor& audioProcessor;

    juce::Slider panLFOSlider;
    RotarySlider panWidthSlider; 
    PanRotarySlider panCenterSlider;

    juce::ToggleButton panWidthLFOToggle, panLFOToggle;
    LFOSliderListenerLabel bpmLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panLFOSliderListener;
    std::unique_ptr<juce::ParameterAttachment> panWidthSliderListener, panCenterSliderListener;

    juce::AudioParameterFloat* panCenterValue;
    juce::ParameterAttachment panCenterSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
