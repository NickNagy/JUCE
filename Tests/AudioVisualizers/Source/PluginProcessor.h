/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioVisualizers.h"
#include "ThreadFunctions.h"//spin_lock

//==============================================================================
/**
*/
class AudioVisualizersAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioVisualizersAudioProcessor();
    ~AudioVisualizersAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setVisualizerPointer(juce::AudioVisualiserComponent* visualizerPtr);

private:
    //==============================================================================
    std::vector<juce::Colour> audioVisualizerColors{juce::Colours::black, juce::Colours::green, juce::Colours::cyan, juce::Colours::orange, juce::Colours::purple};
    std::unique_ptr<juce::AudioVisualiserComponent*> visualizerPtr; //= nullptr;

    spin_lock mutex;
    int totalChannels = 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioVisualizersAudioProcessor)
};
