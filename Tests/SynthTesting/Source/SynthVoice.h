/*
  ==============================================================================

    SynthVoice.h
    Created: 4 Dec 2020 3:56:43pm
    Author:  Nick Nagy

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
//#include "maximilian.h"

class SynthVoice : public juce::SynthesiserVoice {
public:

    bool canPlaySound(juce::SynthesiserSound* sound) {
        // try to cast sound to synthsound* type. If the cast fails, it will return false
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
        adsr.noteOn();
        
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        osc.setFrequency(frequency);
        
        //std::cout << midiNoteNumber << std::endl;
        juce::Logger::outputDebugString(std::to_string(midiNoteNumber));
    }

    void stopNote(float velocity, bool allowTailOff) {
        adsr.noteOff();
    }

    void pitchWheelMoved(int newPitchWheelValue) {
        
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {

        adsr.setSampleRate(sampleRate);

        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = samplesPerBlock;
        spec.sampleRate = sampleRate;
        spec.numChannels = outputChannels;

        osc.prepare(spec);
        gain.prepare(spec);

        gain.setGainLinear(0.01f);

        isPrepared = true;
    }

    void controllerMoved(int controllerNumber, int newControllerValue) {

    }

    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        jassert(isPrepared);
        
        // for DSP, use AudioBlock
        // it's an alias for the buffer it's passed (not copied data, points to same data)
        juce::dsp::AudioBlock<float> audioBlock(outputBuffer);

        // process context
        //  replacing: output of whatever is processing will replace what was in the buffer prior
        //  non-replacing: output of whatever is processing buffer will NOT replace the input
        osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
        gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

        adsr.applyEnvelopeToBuffer(outputBuffer, startSample, numSamples);
    }

private:
    float frequency;
    bool isPrepared = false;

    //==============================================================================
    // oscillator
    // sine wave
    juce::dsp::Oscillator<float> osc{ [](float x) {return std::sin(x); } };
    // juce::dsp::Oscillator<float> saw{ [](float x) {return x / juce::MathConstants<float>::pi; } };
    // juce::dsp::Oscillator<float> sqare { [](float x) { return (x > 0) ? 1.0 : -1.0 ;} };
    juce::dsp::Gain<float> gain;
    
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
};