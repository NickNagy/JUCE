#include "WoflmakerEditorComponents.h"

//========================================= ROTARY SLIDERS =============================================//

WoflRotarySlider::WoflRotarySlider() {
    magna::Component::setColour(ROTARY_SLIDERBODYFILLCOLOUR_STR, WOFL_ROTARY_BODYCOLOUR);
    magna::Component::setColour(ROTARY_THUMBCOLOUR_STR, WOFL_ROTARY_THUMBCOLOUR);
}

WoflPanRotarySlider::WoflPanRotarySlider() {
    magna::Component::setColour(ROTARY_SLIDERBODYFILLCOLOUR_STR, WOFL_ROTARY_BODYCOLOUR);
    magna::Component::setColour(ROTARY_THUMBCOLOUR_STR, WOFL_ROTARY_THUMBCOLOUR);
    magna::Component::setColour(PAN_ROTARY_SLIDERWIDTHRANGECOLOUR_STR, WOFL_PAN_ROTARY_WIDTHINDICATORCOLOUR_LFO_ENABLED);
    //setComponentEffect(&glow);
}

//========================================= LOOK AND FEEL ==============================================//

// Main app
WoflmakerLookAndFeel::WoflmakerLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, WOFL_APP_BACKGROUNDCOLOUR);
    setColour(juce::Label::textColourId, WOFL_APP_TITLE_TEXTCOLOUR);
}

// Slider boxes
SliderBoxLookAndFeel::SliderBoxLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setColour(juce::Label::textColourId, juce::Colours::yellow);
}