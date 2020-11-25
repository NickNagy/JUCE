#include "ComponentBox.h"

namespace magna {
	void RotarySliderBox::resized() {
		auto width = getWidth();
		auto height = getHeight();
		auto x = getX();
		auto y = getY();

		boxTitle.setBounds(x, y, width, TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT * height);

		auto heightFreeForSlider = height * (1.0 - TITLE_HEIGHT_AS_PROPORTION_OF_BOX_HEIGHT - TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT);
		auto diameter = (width < heightFreeForSlider) ? width * ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN : height * ROTARY_DIAMETER_AS_PROPORTION_OF_BOX_MIN_SPAN;
		// TODO: set slider bounds

		slider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, width, TEXTBOX_HEIGHT_AS_PROPORTIOIN_OF_BOX_HEIGHT* height);
	}
}