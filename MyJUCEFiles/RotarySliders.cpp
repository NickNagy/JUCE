#include "RotarySliders.h"

// I chose to define all RotarySlider (base class) functions inside the header file, and only define derived class functions here

//=========================== PAN ROTARY SLIDER ===========================================
PanRotarySlider::PanRotarySlider() {
	setRange(-PAN_MAX_MAGNITUDE, PAN_MAX_MAGNITUDE, 1);
	currentValue.setValue(0);
	minValue.setValue(0);
	maxValue.setValue(0);
	minValue.addListener(this);
	maxValue.addListener(this);
}

PanRotarySlider::~PanRotarySlider() {
	minValue.removeListener(this);
	maxValue.removeListener(this);
}

void PanRotarySlider::setValue(double newValue, juce::NotificationType notification = juce::sendNotificationAsync) {
	newValue = constrainedValue(newValue);
	if (newValue != lastCurrentValue) {
		if (valueBox != nullptr) {
			valueBox->hideEditor(true);
			lastCurrentValue = newValue;
			if (currentValue != newValue) {
				auto halfWidth = (float)maxValue.getValue() - (float)currentValue.getValue();
				// if minValue and maxValue stay in bounds with newVal, update -- otherwise, do nothing
				if (newValue + halfWidth <= (float)PAN_MAX_MAGNITUDE && newValue - halfWidth >= (float)-PAN_MAX_MAGNITUDE) {
					currentValue = newValue;
					maxValue = newValue + halfWidth;
					minValue = newValue - halfWidth;
					// any reason to call these if not updating values?
					updateText();
					repaint();
					triggerChangeMessage(notification);
				}
			}
		}
	}
}

void PanRotarySlider::drawRotarySlider(juce::Graphics&g, float sliderPos) {
	auto width = sliderRect.getWidth();
	auto height = sliderRect.getHeight();
	auto x = sliderRect.getX();
	auto y = sliderRect.getY();
	auto rotaryStartAngle = rotaryParams.startAngleRadians;
	auto rotaryEndAngle = rotaryParams.endAngleRadians;
	auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
	auto centreX = (float)x + (float)width * 0.5f;
	auto centreY = (float)y + (float)height * 0.5f;
	auto rx = centreX - radius;
	auto ry = centreY - radius;
	auto rw = radius * 2.0f;
	auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

	g.setColour(findColour(rotarySliderFillColourId));
	g.fillEllipse(rx, ry, rw, rw);

	g.setColour(findColour(rotarySliderOutlineColourId));
	g.drawEllipse(rx, ry, rw, rw, 1.0f);

	juce::Path p;
	auto pointerLength = radius * 0.33f;
	auto pointerThickness = 2.0f;
	p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

	g.setColour(findColour(thumbColourId));
	g.fillPath(p);

	// if slider's range is controlled by an external slider, draw the range
	if (hasExternalWidthController) {
		juce::Path panRangeArc;
		float panRangeArcThickness = 2.0f;
		auto minValuePos = valueToProportionOfLength(minValue.getValue());
		auto maxValuePos = valueToProportionOfLength(maxValue.getValue());
		auto rangeMinAngle = rotaryStartAngle + minValuePos * (rotaryEndAngle - rotaryStartAngle);
		auto rangeMaxAngle = rotaryStartAngle + maxValuePos * (rotaryEndAngle - rotaryStartAngle);
		panRangeArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rangeMinAngle, rangeMaxAngle, true);
		g.setColour(juce::Colours::green);
		g.strokePath(panRangeArc, juce::PathStrokeType(panRangeArcThickness, juce::PathStrokeType::curved, juce::PathStrokeType::square));
	}
}

void PanRotarySlider::sliderValueChanged(RotarySlider* slider) {
	// code breaks if range of controlling slider is greater than this one
	jassert(slider->getValue() <= 2 * PAN_MAX_MAGNITUDE);
	// for the moment, I am ignoring the case where more than one external slider tries to control a single pan slider
	hasExternalWidthController = true;
	auto halfWidth = 0.5*slider->getValue();
	auto nextMinValue = (float)currentValue.getValue() - halfWidth;
	auto nextMaxValue = (float)currentValue.getValue() + halfWidth;
	// check case where values may go out of bounds of slider's full range
	if (nextMinValue < -PAN_MAX_MAGNITUDE) {
		minValue.setValue((float)-PAN_MAX_MAGNITUDE);
		maxValue.setValue((float)-PAN_MAX_MAGNITUDE + 2.0 * halfWidth);
		setValue((float)-PAN_MAX_MAGNITUDE + halfWidth);
	}
	else if (nextMaxValue > PAN_MAX_MAGNITUDE) {
		minValue.setValue((float)PAN_MAX_MAGNITUDE - 2.0 * halfWidth);
		maxValue.setValue((float)PAN_MAX_MAGNITUDE);
		setValue((float)PAN_MAX_MAGNITUDE - halfWidth);
	}
	else { // don't need to update currentValue
		minValue.setValue(nextMinValue);
		maxValue.setValue(nextMaxValue);
		repaint(); // other two cases don't need to call repaint -- gets called in setValue()
	}
}

void PanRotarySlider::handleRotaryDrag(const juce::MouseEvent& e) {
	auto dx = e.position.x - (float)sliderRect.getCentreX();
	auto dy = e.position.y - (float)sliderRect.getCentreY();
	if (dx * dx + dy * dy > 25.0f) {
		auto angle = std::atan2((double)dx, (double)-dy);
		while (angle < 0.0) {
			angle += juce::MathConstants<double>::twoPi;
		}

		float minLegalAngle = rotaryParams.startAngleRadians;
		float maxLegalAngle = rotaryParams.endAngleRadians;

		if(hasExternalWidthController && (maxValue.getValue() != minValue.getValue())) {
			auto halfWidthAngle = ((double)maxValue.getValue() - (double)currentValue.getValue()) / (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
			minLegalAngle = rotaryParams.startAngleRadians + halfWidthAngle; //valueToProportionOfLength(-PAN_MAX_MAGNITUDE + halfWidth);
			maxLegalAngle = rotaryParams.endAngleRadians - halfWidthAngle;//valueToProportionOfLength(PAN_MAX_MAGNITUDE - halfWidth);
		}

		angle = limitAngleForRotaryDrag(e, angle, minLegalAngle, maxLegalAngle);
		
		auto proportion = (angle - rotaryParams.startAngleRadians) / (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
		valueWhenLastDragged = proportionOfLengthToValue(juce::jlimit(0.0, 1.0, proportion));
		lastAngle = angle;
	}
}