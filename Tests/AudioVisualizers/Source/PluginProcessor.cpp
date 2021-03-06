/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioVisualisersAudioProcessor::AudioVisualisersAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

}

AudioVisualisersAudioProcessor::~AudioVisualisersAudioProcessor()
{
}

//==============================================================================
const juce::String AudioVisualisersAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioVisualisersAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioVisualisersAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioVisualisersAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioVisualisersAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioVisualisersAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioVisualisersAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioVisualisersAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AudioVisualisersAudioProcessor::getProgramName (int index)
{
    return {};
}

void AudioVisualisersAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudioVisualisersAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AudioVisualisersAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioVisualisersAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioVisualisersAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    int newTotalChannels = totalNumInputChannels + totalNumOutputChannels;

    /* using this buffer to write to output w/o writing over input yet */
    juce::AudioBuffer<float> bufferInOutSplit(newTotalChannels, numSamples);
    bufferInOutSplit.clear();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, numSamples);
    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        bufferInOutSplit.copyFrom(channel, 0, buffer.getReadPointer(channel), numSamples);
        // do things to output...
        if (channel + totalNumInputChannels < totalChannels) { // if corresponding output channel exists...
            bufferInOutSplit.copyFrom(channel + totalNumInputChannels, 0, channelData, numSamples); // copy to bufferInOutSplit
        }
    }

    // only do anything with visualizer if it's safe (not actively being re-assigned)
    std::unique_lock<spin_lock> tryLock(mutex, std::try_to_lock);
    if (tryLock.owns_lock()) {
        if (visualiserPtr.get() != nullptr) {
            if (newTotalChannels != totalChannels) // buffer (itself) is only 2-channels
                (*visualiserPtr)->setNumChannels(newTotalChannels);// + totalNumOutputChannels);
            (*visualiserPtr)->pushBuffer(bufferInOutSplit);
        }
    }

    totalChannels = newTotalChannels;
}

//==============================================================================
bool AudioVisualisersAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioVisualisersAudioProcessor::createEditor()
{
    return new AudioVisualisersAudioProcessorEditor (*this);
}

//==============================================================================
void AudioVisualisersAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AudioVisualisersAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioVisualisersAudioProcessor();
}

void AudioVisualisersAudioProcessor::setVisualiserPointer(AudioVisualiser * visualiserPtr) {
    std::lock_guard<spin_lock> lock(mutex); // un-locks mutex once out of scope
    this->visualiserPtr = std::make_unique<AudioVisualiser*>(visualiserPtr);
}
