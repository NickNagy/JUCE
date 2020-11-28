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

#define ROTARY_DIAMETER_AS_PROPORTION_OF_ROTARY_SPACE 0.9f

	struct RotarySliderBox : public ComponentBox {
		magna::RotarySlider& slider;

		RotarySliderBox(RotarySlider& s) : slider(s) {}

		void resized() override;
	};
}

#endif