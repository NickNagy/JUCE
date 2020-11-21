/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WoflmakerAudioProcessorEditor::WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor& p, juce::AudioProcessorValueTreeState& params)
    : AudioProcessorEditor (&p), audioProcessor (p), panCenterValue(new juce::AudioParameterFloat("center", "Pan Center Angle", (float)-PAN_MAX_MAGNITUDE, (float)PAN_MAX_MAGNITUDE, 0.0f)),
    panCenterSliderAttachment(*panCenterValue, nullptr, nullptr)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 600);

    // pan width
    panWidthSlider;//.setSliderStyle(juce::Slider::LinearHorizontal);
    panWidthSlider.setRange(0, 2*PAN_MAX_MAGNITUDE, 2);
    panWidthSlider.addListener(&panCenterSlider);
    addAndMakeVisible(panWidthSlider);

    //pan center
    addAndMakeVisible(panCenterSlider);

    // pan LFO
    panLFOSlider.setSliderStyle(juce::Slider::Rotary);
    panLFOSlider.setRange(0, 60, 0.1f);
    panLFOSlider.setTextValueSuffix("Hz");
    panLFOSlider.addListener(&bpmLabel);
    panLFOSliderListener.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params, "panRate", panLFOSlider));
    addAndMakeVisible(panLFOSlider);

    // LFO BPM Label
    addAndMakeVisible(bpmLabel);

    p.addParameter(panCenterValue);
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

    // todo
    bpmLabel.setBounds(0.5f * (width - textBoxWidth), height - textBoxHeight, textBoxWidth, textBoxHeight);

    auto rSliderRowHeight = 0.33f * height; // 2 rows

    // for choosing radius of slider
    int rSliderOffset = 10;
    auto radius = (width > rSliderRowHeight - textBoxHeight) ? (rSliderRowHeight - textBoxHeight - rSliderOffset)*0.5f : (width - rSliderOffset)*0.5f;
    
    int sliderX = 0.5f * width - radius;
    int topSliderY = 0.16f * height - radius;
    int midSliderY = 0.5f * height - radius;
    int bottomSliderY = 0.84f*height - radius;

    panWidthSlider.setBounds(sliderX, topSliderY, 2 * radius, 2 * radius);
    panWidthSlider.setTextBoxStyle(RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    panCenterSlider.setBounds(sliderX, midSliderY, 2 * radius, 2 * radius);
    panCenterSlider.setTextBoxStyle(RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    panLFOSlider.setBounds(sliderX, bottomSliderY, 2 * radius, 2 * radius);
    panLFOSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, textBoxWidth, textBoxHeight);
}
