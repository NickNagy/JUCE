/*
  ==============================================================================

    SynthSound.h
    Created: 4 Dec 2020 3:56:33pm
    Author:  Nick Nagy

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int midiNoteNumber) { return true; }
    bool appliesToChannel(int midiChannel) { return true; }
};