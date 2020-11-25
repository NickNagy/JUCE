/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotarySliders.h"
#include "Attachments.h"

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
class WoflmakerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	WoflmakerAudioProcessorEditor(WoflmakerAudioProcessor&, juce::AudioProcessorValueTreeState& params, juce::AudioParameterInt* panCenterParameter, juce::AudioParameterInt* panWidthParameter, juce::AudioParameterFloat* panCenterLFOParameter, juce::AudioParameterFloat* panWidthLFOParameter);
	~WoflmakerAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WoflmakerAudioProcessor& audioProcessor;

	// Sliders
	magna::RotarySlider panWidthSlider, panCenterLFOSlider, panWidthLFOSlider;
	magna::PanRotarySlider panCenterSlider;

	// Toggle Buttons
	juce::ToggleButton panWidthLFOToggle, panLFOToggle;

	// Attachments
	magna::RotarySliderParameterAttachment panCenterSliderAttachment, panWidthSliderAttachment, panCenterLFOSliderAttachment, panWidthLFOSliderAttachment;

	// Labels
	LFOSliderListenerLabel bpmLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
