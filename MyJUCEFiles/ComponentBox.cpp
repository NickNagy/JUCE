#include "ComponentBox.h"

namespace magna {
	void RotarySliderBox::resized() {
		auto width = getWidth();
		auto height = getHeight();
		auto x = getX();
		auto y = getY();

		// Box title
		auto boxTitleYOffset = y;
		auto boxTitleHeight = height * ROTARY_TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT;
		boxTitle.setBounds(x, boxTitleYOffset, width, boxTitleHeight);

		// Slider
		auto heightFreeForSlider = height * (1.0 - ROTARY_TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT - ROTARY_TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT);
		auto diameter = (width < heightFreeForSlider) ? width * ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN : height * ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN;
		auto sliderYOffset = boxTitleYOffset + (heightFreeForSlider - diameter) * 0.5f;
		auto sliderXOffset = x + (width - diameter) * 0.5f;
		slider.setBounds(sliderXOffset, sliderYOffset, diameter, diameter);

		// Slider textbox
		slider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, diameter, ROTARY_TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT* height);
	}
}