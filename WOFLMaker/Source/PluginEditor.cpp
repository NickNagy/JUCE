/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WoflmakerAudioProcessorEditor::WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor& p, AudioProcessorValueTreeStateExtended& params)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 600);

    // pan width
    panWidthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    panWidthSlider.setRange(0, 2*MAX_PAN_ANGLE_MAGNITUDE, 2);
    panWidthSlider.setTextValueSuffix("°");
    //panWidthSlider.addListener(&panCenterSlider);
    panWidthSliderListener.reset(new AudioProcessorValueTreeStateExtended::SliderAttachment(params, "width", panWidthSlider));
    addAndMakeVisible(panWidthSlider);

    //test
    testRotarySliderListener.reset(new AudioProcessorValueTreeStateExtended::RotarySliderAttachment(params, "test", testRotarySlider));
    addAndMakeVisible(testRotarySlider);

    // pan center
    //addAndMakeVisible(panCenterSlider);

    // pan LFO
    panLFOSlider.setSliderStyle(juce::Slider::Rotary);
    panLFOSlider.setRange(0, 60, 0.1f);
    panLFOSlider.setTextValueSuffix("Hz");
    panLFOSlider.addListener(&bpmLabel);
    addAndMakeVisible(panLFOSlider);

    // LFO BPM Label
    addAndMakeVisible(bpmLabel);
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
    bpmLabel.setBounds(0.5f * (width - textBoxWidth), height - textBoxHeight - 5, textBoxWidth, textBoxHeight);

    // row for horizontal sliders
    int hSliderOffset = 10;
    int hSliderRowHeight = 60;
    panWidthSlider.setBounds(hSliderOffset * 0.5f, hSliderOffset * 0.5f, width - hSliderOffset, hSliderRowHeight - textBoxHeight - hSliderOffset);
    panWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    height -= hSliderRowHeight;
    auto rSliderRowHeight = 0.5f * height; // 2 rows

    // for choosing radius of slider
    int rSliderOffset = 10;
    auto radius = (width > rSliderRowHeight - textBoxHeight) ? (rSliderRowHeight - textBoxHeight - rSliderOffset)*0.5f : (width - rSliderOffset)*0.5f;
    
    int sliderX = 0.5f * width - radius;
    int topSliderY = hSliderRowHeight + (0.25f * height - radius);
    int bottomSliderY = hSliderRowHeight + (0.75f*height - radius);

    testRotarySlider.setBounds(sliderX, topSliderY, 2 * radius, 2 * radius);
    testRotarySlider.setTextBoxStyle(RotarySlider::TextBoxBelow, true, textBoxWidth, textBoxHeight);

    panLFOSlider.setBounds(sliderX, bottomSliderY, 2 * radius, 2 * radius);
    panLFOSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, textBoxWidth, textBoxHeight);
}
