/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WoflmakerAudioProcessor::WoflmakerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
     ), tree(*this, nullptr, "PARAMETERS", {}), panCenterParameter("center", "Pan Center", (float)-PAN_MAX_MAGNITUDE, (float)PAN_MAX_MAGNITUDE, 0.0f)
#endif
{
    addParameter(&panCenterParameter);

    using Parameter = juce::AudioProcessorValueTreeState::Parameter;

    panWidthLFO.initialise([](float x) { return std::sin(x); }, 128);
    panWidthLFO.setFrequency(0.0f);
}

WoflmakerAudioProcessor::~WoflmakerAudioProcessor()
{
}

//==============================================================================
const juce::String WoflmakerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WoflmakerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WoflmakerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WoflmakerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WoflmakerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WoflmakerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WoflmakerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WoflmakerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WoflmakerAudioProcessor::getProgramName (int index)
{
    return {};
}

void WoflmakerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WoflmakerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    tempBlock = juce::dsp::AudioBlock<float>(heapBlock, getTotalNumInputChannels(), samplesPerBlock);
    //panLFO.prepare({ sampleRate / lfoUpdateRate, samplesPerBlock, getTotalNumInputChannels() });
}

void WoflmakerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WoflmakerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WoflmakerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    float gainL, gainR;

    // channel gains
    auto p = juce::MathConstants<float>::halfPi * 0.5 * ((panCenterParameter.get() / (float)PAN_MAX_MAGNITUDE)+ 1.0);
    gainL = std::cos(p);
    gainR = std::sin(p);

    // left channel
    if (gainL == previousGainL) {
        buffer.applyGain(0, 0, numSamples, gainL);
    }
    else {
        buffer.applyGainRamp(0, 0, numSamples, previousGainL, gainL);
        previousGainL = gainL;
    }

    // gainR
    if (totalNumInputChannels > 1) {
        if (gainR == previousGainR) {
            buffer.applyGain(1, 0, numSamples, gainR);
        }
        else {
            buffer.applyGainRamp(1, 0, numSamples, previousGainR, gainR);
            previousGainR = gainR;
        }
    }
}

//==============================================================================
bool WoflmakerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WoflmakerAudioProcessor::createEditor()
{
    return new WoflmakerAudioProcessorEditor (*this, tree, panCenterParameter);
}

//==============================================================================
void WoflmakerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WoflmakerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WoflmakerAudioProcessor();
}
