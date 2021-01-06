/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../MyJUCEFiles/RotarySliders.h" // for PAN_MAX_MAGNITUDE

#define MAX_LFO_FREQUENCY_HZ 60

//==============================================================================
/**
*/
class WoflmakerAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	WoflmakerAudioProcessor();
	~WoflmakerAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

private:
	//==============================================================================
	juce::AudioProcessorValueTreeState tree;

	// Tracking previous states
	float previousGainL = 1.0;
	float previousGainR = 1.0;

	// LFOs
	juce::dsp::Oscillator<float> panCenterLFO, panWidthLFO;
	static constexpr size_t lfoUpdateRate = 100;
	size_t lfoUpdateCounter = lfoUpdateRate;

	// Processor parameters
	juce::AudioParameterInt* panCenterParameter, * panWidthParameter, * panCenterLFOFunctionMenuChoiceParameter;
	juce::AudioParameterFloat* panCenterLFOParameter, * panWidthLFOParameter;
	juce::AudioParameterBool* panCenterLFOToggleParameter;

	void setLFOFunction(juce::dsp::Oscillator<float>& lfo, int function);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WoflmakerAudioProcessor)
};
