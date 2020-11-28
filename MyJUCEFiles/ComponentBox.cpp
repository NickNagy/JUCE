#include "ComponentBox.h"

namespace magna {
	// TODO: rectangle subdivision
	void RotarySliderBox::resized() {
		auto area = getBounds();

		auto boxTitleHeight = 30;
		auto textBoxHeight = 20;

		boxTitle.setBounds(area.removeFromTop(boxTitleHeight));
		area.removeFromBottom(textBoxHeight);
		auto w = area.getWidth();
		auto h = area.getHeight();
		auto minDimension = juce::jmin(w, h);
		auto sliderDiameter = ROTARY_DIAMETER_AS_PROPORTION_OF_ROTARY_SPACE * minDimension;
		area.reduced(minDimension - sliderDiameter);
		slider.setBounds(area);
		slider.setTextBoxStyle(magna::RotarySlider::TextBoxBelow, true, sliderDiameter, textBoxHeight);
	}
}