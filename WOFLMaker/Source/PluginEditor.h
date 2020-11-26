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
#include "ComponentBox.h"

/* To convert the LFO slider(s)' Hz value to a BPM value and display it */
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

//====================================================== ROTARY SLIDERS ==================================================//

/* derived from RotarySliders.h, to override colors in constructors for app customization*/
class WoflRotarySlider : public magna::RotarySlider {
public:
	WoflRotarySlider();
private:
};

class WoflPanRotarySlider : public magna::RotarySlider {
public:
	WoflPanRotarySlider();
};

//======================================================= LOOK AND FEEL ==================================================//

/* Here are custom lookAndFeels for juce components in the app. These do not affect the designs of magna components */

// Main app
struct WoflmakerLookAndFeel : public juce::LookAndFeel_V4 {
	WoflmakerLookAndFeel();
};

struct SliderBoxLookAndFeel : public juce::LookAndFeel_V4 {
	SliderBoxLookAndFeel();
};

#define SLIDERS_PER_COLUMN 3
#define SLIDERS_PER_ROW 1
/* Proportional height macros should sum up to approx. 1.0 */
#define APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.2f
// height of space of all rows and columns of sliders
#define SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.6f
#define LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.2f

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

	// Look and feel
	WoflmakerLookAndFeel appLookAndFeel;
	SliderBoxLookAndFeel sliderBoxLookAndFeel;

	// Title for app
	juce::Label appTitle;

	// Sliders
	magna::RotarySlider panWidthSlider, panCenterLFOSlider, panWidthLFOSlider;
	magna::PanRotarySlider panCenterSlider;

	// Slider attachments
	magna::RotarySliderParameterAttachment panCenterSliderAttachment, panWidthSliderAttachment, panCenterLFOSliderAttachment, panWidthLFOSliderAttachment;

	// Toggle Buttons
	juce::ToggleButton panWidthLFOToggle, panLFOToggle;

	// Component boxes
	magna::RotarySliderBox panWidthSliderBox, panCenterSliderBox, panCenterLFOSliderBox;

	// Other
	LFOSliderListenerLabel bpmLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
