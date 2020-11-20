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

#define MAX_PAN_ANGLE_MAGNITUDE 45.0f
#define DEGREES_TO_RADIANS(d) d*juce::MathConstants<double>::pi/180.0
#define RADIANS_TO_DEGREES(r) r*180.0/juce::MathConstants<double>::pi;

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

    juce::Slider panWidthSlider, panLFOSlider;
    //PanRotarySlider panCenterSlider;

    RotarySlider testRotarySlider;

    juce::ToggleButton panWidthLFOToggle, panLFOToggle;
    LFOSliderListenerLabel bpmLabel;
    std::unique_ptr<AudioProcessorValueTreeStateExtended::SliderAttachment> panWidthSliderListener, panCenterSliderListener, panLFOSliderListener;
    std::unique_ptr<AudioProcessorValueTreeStateExtended::RotarySliderAttachment> testRotarySliderListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
