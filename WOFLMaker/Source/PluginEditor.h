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

/* Proportional height macros should sum up to approx. 1.0 */
#define APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.2f
// height of slider box w.r.t app height
#define SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.3f
/* other dimension macros */
#define LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.2f
#define SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.33f
#define TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.1f
#define CONTROLBOX_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.5f
#define TOGGLEBUTTON_WIDTH_AS_PROPORTION_OF_CONTROLBOX_WIDTH 0.33f

#define CONTROLBOX_SLIDERBOX_ROWS 1
#define CONTROLBOX_SLIDERBOX_COLS 2

#define NUM_LFO_FUNCTIONS 4

//==============================================================================
/**
*/
class WoflmakerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	WoflmakerAudioProcessorEditor(WoflmakerAudioProcessor&, juce::AudioProcessorValueTreeState& params, juce::AudioParameterInt* panCenterParameter, 
		juce::AudioParameterInt* panWidthParameter, juce::AudioParameterFloat* panCenterLFOParameter, juce::AudioParameterFloat* panWidthLFOParameter, 
		juce::AudioParameterBool* panCenterLFOToggleParameter, juce::AudioParameterInt* panCenterLFOFunctionParameter);
	~WoflmakerAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// Control box struct
	struct LFOControlBox : juce::Component {
		magna::RotarySliderBox& widthSliderBox, &lfoSliderBox;
		juce::ToggleButton& toggleButton;
		juce::ComboBox& dropDownMenu;
		LFOControlBox(magna::RotarySliderBox& widthSB, magna::RotarySliderBox& lfoSB, juce::ToggleButton& button, juce::ComboBox& menu) : widthSliderBox(widthSB), lfoSliderBox(lfoSB), toggleButton(button), dropDownMenu(menu) {}
		void resized() override;
	};
	LFOControlBox controlBox;
	
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WoflmakerAudioProcessor& audioProcessor;

	// Look and feel
	WoflmakerLookAndFeel appLookAndFeel;
	SliderBoxLookAndFeel sliderBoxLookAndFeel;

	// Title for app
	juce::Label appTitle;

	// Toggle for LFO
	juce::ToggleButton panCenterLFOToggleButton;

	// Toggle attachment
	juce::ButtonParameterAttachment panCenterLFOToggleButtonAttachment;

	// Drop-down menu for LFO function selection
	juce::ComboBox panCenterLFOFunctionMenu;

	// Drop-down menu attachments
	juce::ComboBoxParameterAttachment panCenterLFOFunctionMenuAttachment;

	// Sliders
	magna::RotarySlider panWidthSlider, panCenterLFOSlider, panWidthLFOSlider;
	magna::PanRotarySlider panCenterSlider;

	// Slider attachments
	magna::RotarySliderParameterAttachment panCenterSliderAttachment, panWidthSliderAttachment, panCenterLFOSliderAttachment, panWidthLFOSliderAttachment;

	// Component boxes
	magna::RotarySliderBox panWidthSliderBox, panCenterSliderBox, panCenterLFOSliderBox;

	// Other
	LFOSliderListenerLabel bpmLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
