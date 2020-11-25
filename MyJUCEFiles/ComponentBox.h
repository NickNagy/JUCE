#ifndef COMPONENTBOX_H
#define COMPONENTBOX_H

#include <JuceHeader.h>
#include "RotarySliders.h"

namespace magna {
	
#define TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT 0.15f
#define TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT 0.1f
#define ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN 0.5f
	struct RotarySliderBox : public juce::Component {
		juce::Label boxTitle;
		magna::RotarySlider& slider;
		void resized() override;
	};
}

#endif