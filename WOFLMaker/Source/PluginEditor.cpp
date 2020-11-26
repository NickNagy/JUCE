/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//========================================= ROTARY SLIDERS =============================================//

WoflRotarySlider::WoflRotarySlider() {
    setColour(rotarySliderOutlineColourId, juce::Colours::black);
    setColour(thumbColourId, juce::Colours::aquamarine);
}

WoflPanRotarySlider::WoflPanRotarySlider() {
}

//========================================= LOOK AND FEEL ==============================================//

// Main app
WoflmakerLookAndFeel::WoflmakerLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::darkslategrey);
    setColour(juce::Label::textColourId, juce::Colours::orange);
}

// Slider boxes
SliderBoxLookAndFeel::SliderBoxLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setColour(juce::Label::textColourId, juce::Colours::yellow);
}

//======================================== APP ===========================================================//
WoflmakerAudioProcessorEditor::WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor& p, juce::AudioProcessorValueTreeState& params, juce::AudioParameterInt * panCenterParameter, juce::AudioParameterInt * panWidthParameter,
    juce::AudioParameterFloat * panCenterLFOParameter, juce::AudioParameterFloat * panWidthLFOParameter)
    : AudioProcessorEditor (&p), audioProcessor (p), panCenterSliderAttachment(*panCenterParameter, panCenterSlider, nullptr), panWidthSliderAttachment(*panWidthParameter, panWidthSlider, nullptr),
    panCenterLFOSliderAttachment(*panCenterLFOParameter, panCenterLFOSlider, nullptr), panWidthLFOSliderAttachment(*panWidthLFOParameter, panWidthLFOSlider, nullptr),
    panCenterSliderBox(panCenterSlider), panWidthSliderBox(panWidthSlider), panCenterLFOSliderBox(panCenterLFOSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 600);

    // App title
    appTitle.setText("wOFLPAN", juce::NotificationType::dontSendNotification);
    appTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(appTitle);

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

    // Component boxes: to add titles to sliders and simplify placement in window
    panCenterSliderBox.boxTitle.setText("Pan Center", juce::NotificationType::dontSendNotification);
    panWidthSliderBox.boxTitle.setText("Pan Width", juce::NotificationType::dontSendNotification);
    panCenterLFOSliderBox.boxTitle.setText("Pan Oscillation Rate", juce::NotificationType::dontSendNotification);
    // Don't know if it is unsafe to pass box struct labels to addAndMakeVisible() ?
    addAndMakeVisible(panCenterSliderBox.boxTitle);
    addAndMakeVisible(panWidthSliderBox.boxTitle);
    addAndMakeVisible(panCenterLFOSliderBox.boxTitle);

    // set look and feel
    setLookAndFeel(&appLookAndFeel);
    
    /*panCenterSliderBox.setLookAndFeel(&sliderBoxLookAndFeel);
    panWidthSliderBox.setLookAndFeel(&sliderBoxLookAndFeel);
    panCenterLFOSliderBox.setLookAndFeel(&sliderBoxLookAndFeel);*/
}

WoflmakerAudioProcessorEditor::~WoflmakerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void WoflmakerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
}

void WoflmakerAudioProcessorEditor::resized()
{
    auto width = getWidth();
    auto height = getHeight();

    auto freeYSpace = 1.0f - (APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT);

    // App Title
    auto appTitleYOffset = 0;
    auto appTitleHeight = height * APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT;
    appTitle.setBounds(0, appTitleYOffset, width, appTitleHeight);

    // Slider boxes
    auto firstSliderBoxYOffset = appTitleYOffset + appTitleHeight;
    auto sliderBoxHeight = height * SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT / (float)SLIDERS_PER_COLUMN;
    panWidthSliderBox.setBounds(0, firstSliderBoxYOffset, width, sliderBoxHeight);
    panCenterSliderBox.setBounds(0, firstSliderBoxYOffset + sliderBoxHeight, width, sliderBoxHeight);
    panCenterLFOSliderBox.setBounds(0, firstSliderBoxYOffset + 2 * sliderBoxHeight, width, sliderBoxHeight);

    // LFO window
    auto lfoWindowHeight = height * LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT;
    // TODO
}
