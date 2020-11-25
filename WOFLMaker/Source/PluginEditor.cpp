/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WoflmakerAudioProcessorEditor::WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor& p, juce::AudioProcessorValueTreeState& params, juce::AudioParameterInt * panCenterParameter, juce::AudioParameterInt * panWidthParameter,
    juce::AudioParameterFloat * panCenterLFOParameter, juce::AudioParameterFloat * panWidthLFOParameter)
    : AudioProcessorEditor (&p), audioProcessor (p), panCenterSliderAttachment(*panCenterParameter, panCenterSlider, nullptr), panWidthSliderAttachment(*panWidthParameter, panWidthSlider, nullptr),
    panCenterLFOSliderAttachment(*panCenterLFOParameter, panCenterLFOSlider, nullptr), panWidthLFOSliderAttachment(*panWidthLFOParameter, panWidthLFOSlider, nullptr)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 600);

    // Pan Width slider
    panWidthSlider.setRange(0, (int)PAN_MAX_MAGNITUDE*2, 2);
    panWidthSlider.addListener(&panCenterSlider);
    addAndMakeVisible(panWidthSlider);

    // Pan Center slider
    addAndMakeVisible(panCenterSlider);

    // Pan Width LFO slider
    panWidthLFOSlider.setRange(0, (float)MAX_LFO_FREQUENCY_HZ, 0.1f);
    panWidthLFOSlider.setTextValueSuffix("Hz");

    // Pan Center LFO slider
    panCenterLFOSlider.setRange(0, (float)MAX_LFO_FREQUENCY_HZ, 0.1f);
    panCenterLFOSlider.setTextValueSuffix("Hz");
    addAndMakeVisible(panCenterLFOSlider);
}

WoflmakerAudioProcessorEditor::~WoflmakerAudioProcessorEditor()
{
}

//==============================================================================
void WoflmakerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void WoflmakerAudioProcessorEditor::resized()
{
    auto width = getWidth();
    auto height = getHeight();

    int textBoxHeight = 20;
    int textBoxWidth = 50;

    auto rSliderRowHeight = 0.33f * height; // 2 rows

    // for choosing radius of slider
    int rSliderOffset = 10;
    auto radius = (width > rSliderRowHeight - textBoxHeight) ? (rSliderRowHeight - textBoxHeight - rSliderOffset)*0.5f : (width - rSliderOffset)*0.5f;
    
    int sliderX = 0.5f * width - radius;
    int topSliderY = 0.16f * height - radius;
    int midSliderY = 0.5f * height - radius;
    int bottomSliderY = 0.84f*height - radius;

    panWidthSlider.setBounds(sliderX, topSliderY, 2 * radius, 2 * radius);
    panWidthSlider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    panCenterSlider.setBounds(sliderX, midSliderY, 2 * radius, 2 * radius);
    panCenterSlider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    panCenterLFOSlider.setBounds(sliderX, bottomSliderY, 2 * radius, 2 * radius);
    panCenterLFOSlider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);
}
