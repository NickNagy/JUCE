/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioVisualisersAudioProcessorEditor::AudioVisualisersAudioProcessorEditor (AudioVisualisersAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), visualiser(2, 2, NO_OVERLAY)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 300);

    addAndMakeVisible(visualiser);

    // clear buffer here instead of in prepareToPlay() as editor could be opened/closed at any time
    visualiser.clear();
    visualiser.setSamplesPerBlock(64);
    visualiser.setRepaintRate(20);

    // share visualizer component with processor
    audioProcessor.setVisualiserPointer(&visualiser);
}

AudioVisualisersAudioProcessorEditor::~AudioVisualisersAudioProcessorEditor()
{
    audioProcessor.setVisualiserPointer(nullptr);
}

//==============================================================================
void AudioVisualisersAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    visualiser.paint(g);
}

void AudioVisualisersAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int offset = 10;
    visualiser.setBounds(offset, offset, getWidth() - (offset<<1), getHeight() - (offset<<1));
}
