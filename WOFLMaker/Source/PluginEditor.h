/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WoflmakerEditorStylesheet.h"
#include "WoflmakerEditorComponents.h"
#include "RotarySliders.h"
#include "Attachments.h"
#include "ComponentBox.h"

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
	WoflRotarySlider panWidthSlider, panCenterLFOSlider, panWidthLFOSlider;
	WoflPanRotarySlider panCenterSlider;

	// Slider attachments
	magna::RotarySliderParameterAttachment panCenterSliderAttachment, panWidthSliderAttachment, panCenterLFOSliderAttachment, panWidthLFOSliderAttachment;

	// Component boxes
	magna::RotarySliderBox panWidthSliderBox, panCenterSliderBox, panCenterLFOSliderBox;

	// Other
	LFOSliderListenerLabel bpmLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessorEditor)
};
