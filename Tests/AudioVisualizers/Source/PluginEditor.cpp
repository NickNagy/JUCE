/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioVisualizersAudioProcessorEditor::AudioVisualizersAudioProcessorEditor (AudioVisualizersAudioProcessor& p, AudioVisualizer& v)
    : AudioProcessorEditor (&p), audioProcessor (p), visualizer(v)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(visualizer);
}

AudioVisualizersAudioProcessorEditor::~AudioVisualizersAudioProcessorEditor()
{
}

//==============================================================================
void AudioVisualizersAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    visualizer.paint(g);
}

void AudioVisualizersAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int offset = 10;
    visualizer.setBounds(offset, offset, getWidth() - (offset<<1), getHeight() - (offset<<1));
}
