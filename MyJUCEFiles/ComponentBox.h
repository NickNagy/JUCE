#ifndef COMPONENTBOX_H
#define COMPONENTBOX_H

#include <JuceHeader.h>
#include "RotarySliders.h"

namespace magna {

	struct ComponentBox : public juce::Component {
		juce::Label boxTitle;

		ComponentBox() {
			boxTitle.setJustificationType(juce::Justification::centred);
		}

		// in case setLookAndFeel() gets called during object's lifetime
		~ComponentBox() override {
			setLookAndFeel(nullptr);
		}
	};

#define ROTARY_TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT 0.15f
#define ROTARY_TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT 0.1f
#define ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN 0.5f
	struct RotarySliderBox : public ComponentBox {
		magna::RotarySlider& slider;

		RotarySliderBox(RotarySlider& s) : slider(s) {}

		void resized() override;
	};
}

#endif