/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AnalogFiltersAudioProcessorEditor::AnalogFiltersAudioProcessorEditor (AnalogFiltersAudioProcessor& p, juce::AudioParameterFloat * cutoffFrequencyParameter)
    : AudioProcessorEditor (&p), audioProcessor (p), cutoffFreqSliderAttachment(*cutoffFrequencyParameter, cutoffFreqSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    cutoffFreqSlider.setRange(20.0, 20000.0, 1);
    cutoffFreqSlider.setTextValueSuffix("Hz");
    addAndMakeVisible(cutoffFreqSlider);
}

AnalogFiltersAudioProcessorEditor::~AnalogFiltersAudioProcessorEditor()
{
}

//==============================================================================
void AnalogFiltersAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto area = getBounds();

    g.setColour(juce::Colours::white);
    g.drawFittedText(juce::String("Cutoff Frequency"), area.removeFromTop(sliderLabelHeight), juce::Justification::centred, 2);
}

void AnalogFiltersAudioProcessorEditor::resized()
{
    int sliderTextBoxHeight = 20;

    auto area = getBounds();
    
    area.removeFromTop(sliderLabelHeight);
    area.removeFromBottom(sliderTextBoxHeight);

    cutoffFreqSlider.setBounds(area.getX(), area.getY(), area.getWidth(), area.getHeight());
}
