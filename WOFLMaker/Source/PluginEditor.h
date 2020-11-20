/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotarySliders.h"
#include "AudioProcessorValueTreeStateExtended.h"

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
    WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor&, AudioProcessorValueTreeStateExtended& params);
    ~WoflmakerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WoflmakerAudioProcessor& audioProcessor;

    juce::Slider panLFOSlider;
    //PanRotarySlider panCenterSlider;

    RotarySlider panWidthSlider; 
    PanRotarySlider testRotarySlider;

    juce::ToggleButton panWidthLFOToggle, panLFOToggle;
    LFOSliderListenerLabel bpmLabel;
    std::unique_ptr<AudioProcessorValueTreeStateExtended::SliderAttachment> panCenterSliderListener, panLFOSliderListener;
    std::unique_ptr<AudioProcessorValueTreeStateExtended::RotarySliderAttachment> panWidthSliderListener, testRotarySliderListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
