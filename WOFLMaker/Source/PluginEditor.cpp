/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//======================================== APP ===========================================================//
WoflmakerAudioProcessorEditor::WoflmakerAudioProcessorEditor (WoflmakerAudioProcessor& p, juce::AudioProcessorValueTreeState& params, juce::AudioParameterInt * panCenterParameter, juce::AudioParameterInt * panWidthParameter,
    juce::AudioParameterFloat * panCenterLFOParameter, juce::AudioParameterFloat * panWidthLFOParameter, juce::AudioParameterBool* panCenterLFOToggleParameter, juce::AudioParameterInt* panCenterLFOFunctionParameter)
    : AudioProcessorEditor (&p), audioProcessor (p), panCenterSliderAttachment(*panCenterParameter, panCenterSlider, nullptr), panWidthSliderAttachment(*panWidthParameter, panWidthSlider, nullptr),
    panCenterLFOSliderAttachment(*panCenterLFOParameter, panCenterLFOSlider, nullptr), panWidthLFOSliderAttachment(*panWidthLFOParameter, panWidthLFOSlider, nullptr),
    panCenterSliderBox(panCenterSlider), panWidthSliderBox(panWidthSlider), panCenterLFOSliderBox(panCenterLFOSlider), panCenterLFOToggleButtonAttachment(*panCenterLFOToggleParameter, panCenterLFOToggleButton, nullptr),
    panCenterLFOFunctionMenuAttachment(*panCenterLFOFunctionParameter, panCenterLFOFunctionMenu, nullptr), controlBox(panWidthSliderBox, panCenterLFOSliderBox, panCenterLFOToggleButton, panCenterLFOFunctionMenu)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 600);

    // App title
    appTitle.setText("wOFL\nPan", juce::NotificationType::dontSendNotification);
    appTitle.setFont({ 15.0f, juce::Font::bold & juce::Font::italic });
    appTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(appTitle);

    // Pan Width slider
    panWidthSlider.setRange(0, (int)PAN_MAX_MAGNITUDE*2, 2);
    panWidthSlider.addListener(&panCenterSlider);
    addAndMakeVisible(panWidthSlider);

    // Pan Center slider
    addAndMakeVisible(panCenterSlider);

    // Pan Width LFO slider
    //panWidthLFOSlider.setRange(0, (float)MAX_LFO_FREQUENCY_HZ, 0.1f);
    //panWidthLFOSlider.setTextValueSuffix("Hz");

    // Pan Center LFO slider
    panCenterLFOSlider.setRange(0, (float)MAX_LFO_FREQUENCY_HZ, 0.1f);
    panCenterLFOSlider.setTextValueSuffix("Hz");
    addAndMakeVisible(panCenterLFOSlider);

    // Toggle Button
    panCenterLFOToggleButton.setButtonText("OFF");
    panCenterLFOToggleButton.onClick = [&]() {if (panCenterLFOToggleButton.getButtonText() == "OFF") { panCenterLFOToggleButton.setButtonText("ON"); }
    else { panCenterLFOToggleButton.setButtonText("OFF"); } };
    addAndMakeVisible(&panCenterLFOToggleButton);

    // Drop-down menu to select LFO function
    // TODO

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


// TODO: layout by rectangle subdivision
void WoflmakerAudioProcessorEditor::resized()
{
    jassert(WOFL_CONTROLBOX_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + WOFL_APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT <= 1.0);

    auto area = getBounds();
    auto width = getWidth();
    auto height = getHeight();

    auto freeYSpace = 1.0f - (WOFL_APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT + WOFL_CONTROLBOX_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT);
    // space between title and pan slider, pan slider and controlbox, and controlbox and bottom of app
    freeYSpace /= 3;

    // App Title
    auto appTitleHeight = height * WOFL_APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT;
    auto sliderBoxHeight = height * WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT;
    auto controlBoxHeight = height * WOFL_CONTROLBOX_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT;
    auto controlBoxWidth = width * WOFL_CONTROLBOX_WIDTH_AS_PROPORTION_OF_APP_WIDTH;

    appTitle.setBounds(area.removeFromTop(appTitleHeight));
    area.removeFromTop(freeYSpace);
    area.removeFromBottom(freeYSpace);
    panCenterSliderBox.setBounds(area.removeFromTop(sliderBoxHeight));
    area.removeFromTop(freeYSpace);

    // Control box
    auto widthToRemoveFromSides = 0.5 * (width - controlBoxWidth);
    area.removeFromLeft(widthToRemoveFromSides);
    area.removeFromRight(widthToRemoveFromSides);
    controlBox.setBounds(area);
}

// TODO: layout by rectangle subdivision
void WoflmakerAudioProcessorEditor::LFOControlBox::resized() {
    jassert(WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT + WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT* WOFL_CONTROLBOX_SLIDERBOX_COLS + WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT <= 1.0);

    auto area = getBounds();
    auto width = area.getWidth();
    auto height = area.getHeight();

    // may use to spread out components freely
    auto freeYSpace = 1.0f - (WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT + WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT * WOFL_CONTROLBOX_SLIDERBOX_COLS + WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT);
    // space between title, 2 sliderboxes, lfo window
    freeYSpace /= 4;

    auto sliderBoxSpaceHeight = WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT * height;
    auto sliderBoxHeight = sliderBoxSpaceHeight / WOFL_CONTROLBOX_SLIDERBOX_COLS;
    auto toggleButtonHeight = WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT * height;
    auto toggleButtonWidth = WOFL_TOGGLEBUTTON_WIDTH_AS_PROPORTION_OF_CONTROLBOX_WIDTH * width;
    // for now, top-left
    toggleButton.setBounds(area.removeFromTop(toggleButtonHeight));
    area.removeFromTop(freeYSpace); // betwen title and width sliderbox
    area.removeFromBottom(freeYSpace); // between LFO window and bottom of control box
    widthSliderBox.setBounds(area.removeFromTop(sliderBoxHeight));
    area.removeFromTop(freeYSpace);
    lfoSliderBox.setBounds(area.removeFromTop(sliderBoxHeight));
    // LFO window
    // TODO
}
