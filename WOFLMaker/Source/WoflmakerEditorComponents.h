#ifndef WOFLMAKEREDITORCOMPONENTS_H
#define WOFLMAKEREDITORCOMPONENTS_H

#include <JuceHeader.h>
#include "../../MyJUCEFiles/RotarySliders.h"
#include "WoflmakerEditorStylesheet.h"

/* To convert the LFO slider(s)' Hz value to a BPM value and display it */
class LFOSliderListenerLabel : public juce::Slider::Listener, public juce::Label {
public:
	LFOSliderListenerLabel() {
		setText("0", juce::NotificationType::dontSendNotification);
	};
	~LFOSliderListenerLabel() override {};
private:
	void sliderValueChanged(juce::Slider* slider) override {
		auto toBPM = slider->getValue() / 60.0f;
		setText(JUCE_TO_STRING(toBPM), juce::NotificationType::dontSendNotification);
	}
};

//====================================================== ROTARY SLIDERS ==================================================//

/* derived from RotarySliders.h, to override colors in constructors for app customization*/
class WoflRotarySlider : public magna::RotarySlider {
public:
	WoflRotarySlider();
private:
};

class WoflPanRotarySlider : public magna::PanRotarySlider {
public:
	WoflPanRotarySlider();
private:
	juce::GlowEffect glow;
};

//======================================================= LOOK AND FEEL ==================================================//

/* Here are custom lookAndFeels for juce components in the app. These do not affect the designs of magna components */

// Main app
struct WoflmakerLookAndFeel : public juce::LookAndFeel_V4 {
	WoflmakerLookAndFeel();
};

#endif