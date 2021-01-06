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
    setSize(WOFL_APP_START_WIDTH, appTitleHeight + mainControlBoxHeight + lfoControlBoxExtHeight);

    // App title
    appTitle.setText("wOFL\nPan", juce::NotificationType::dontSendNotification);
    appTitle.setFont({ 25.0f, juce::Font::bold & juce::Font::italic });
    appTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(appTitle);

    // Pan Width slider
    panWidthSlider.setRange(0, (int)PAN_MAX_MAGNITUDE*2, 2);
    panWidthSlider.addListener(&panCenterSlider);
    addAndMakeVisible(panWidthSlider);

    // Pan Center slider
    addAndMakeVisible(panCenterSlider);

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
    g.fillAll(WOFL_APP_BACKGROUNDCOLOUR);//findColour(juce::ResizableWindow::backgroundColourId));

    auto area = getBounds();
    auto height = area.getHeight();
    auto width = area.getWidth();

    auto titleBorderThickness = 5.0f;
    auto defaultBorderThickness = 2.0f;

    // app title
    auto appTitleArea = area.removeFromTop(appTitleHeight);
    g.setColour(juce::Colours::black);
    g.fillRect(appTitleArea);
    g.setColour(juce::Colours::orange);
    g.drawRect(appTitleArea, titleBorderThickness);

    // pan center slider box
    auto panCenterSliderBoxArea = area.removeFromTop(mainControlBoxHeight);
    g.setColour(juce::Colours::grey);
    g.fillRect(panCenterSliderBoxArea);
    g.setColour(juce::Colours::white);
    g.drawRect(panCenterSliderBoxArea, defaultBorderThickness);

    // external area around control box
    g.setColour(juce::Colours::orange);
    g.fillRect(area);

    // control box
    area = area.reduced(lfoControlBoxIntReductionFactor, lfoControlBoxIntReductionFactor);
    g.setColour(juce::Colours::grey);
    g.fillRect(area);
    g.setColour(juce::Colours::black);
    g.drawRect(area, defaultBorderThickness);
}


// TODO: layout by rectangle subdivision
void WoflmakerAudioProcessorEditor::resized()
{
    auto area = getBounds();
    auto height = area.getHeight();
    auto width = area.getWidth();

    appTitle.setBounds(area.removeFromTop(appTitleHeight));
    panCenterSliderBox.setBounds(area.removeFromTop(mainControlBoxHeight));

    controlBox.setBounds(area.reduced(lfoControlBoxIntReductionFactor, lfoControlBoxIntReductionFactor));
}

#if USING_IMAGES
void WoflmakerAudioProcessorEditor::setImages() {
    appBackgroundImage = 
}
#endif

void WoflmakerAudioProcessorEditor::setFonts() {
    appTitleFont = juce::Font(WOFL_GET_APP_TITLE_FONT);
    appComponentTitleFont = juce::Font(WOFL_GET_COMPONENT_TITLE_FONT);
    appDigitalFont = juce::Font(WOFL_GET_DIGITAL_FONT);
}

// ========================================================================= CONTROL BOX ========================================================================== //

void WoflmakerAudioProcessorEditor::LFOControlBox::resized() {
    jassert(WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT + WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT* WOFL_CONTROLBOX_SLIDERBOX_COLS + WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT <= 1.0);

    auto area = getBounds();
    auto width = area.getWidth();
    auto height = area.getHeight();

    auto toggleButtonHeight = 20;
    auto toggleButtonWidth = 60;
    auto lfoWindowHeight = height * WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT;

    toggleButton.setBounds(getX(), getY(), toggleButtonWidth, toggleButtonHeight);
    area.removeFromTop(toggleButtonHeight);
    area.removeFromBottom(lfoWindowHeight); // lfoWindow.setBounds(area.removeFromBottom(lfoWindowHeight).reduced(WOFL_CONTROLBOX_SPACE_LFO_WINDOW_REDUCTION_FACTOR));

    auto sliderBoxHeight = area.getHeight()/2;

    widthSliderBox.setBounds(area.removeFromTop(sliderBoxHeight));
    lfoSliderBox.setBounds(area);
}
