#include "RotarySliders.h"

namespace magna {
	//=========================== ROTARY SLIDER ===============================================//
	RotarySlider::RotarySlider() : textBoxPos(TextEntryBoxPosition::TextBoxBelow) {
		setWantsKeyboardFocus(false);
		setRepaintsOnMouseActivity(true);
		rotaryParams.startAngleRadians = juce::MathConstants<float>::pi * 1.2f;
		rotaryParams.endAngleRadians = juce::MathConstants<float>::pi * 2.8f;
		rotaryParams.stopAtEnd = true;
		lookAndFeelChanged();
		updateText();
		currentValue.addListener(this);
	}

	RotarySlider::~RotarySlider() {
		currentValue.removeListener(this);
	}

	void RotarySlider::setRotaryParameters(RotaryParameters newParameters) noexcept {
		jassert(newParameters.startAngleRadians >= 0 && newParameters.endAngleRadians >= 0);
		jassert(newParameters.startAngleRadians < juce::MathConstants<float>::pi * 4.0f && newParameters.endAngleRadians < juce::MathConstants<float>::pi * 4.0f);
		rotaryParams = newParameters;
	}

	void RotarySlider::setRotaryParameters(float startAngleRadians,float endAngleRadians,bool stopAtEnd) noexcept {
		setRotaryParameters({ startAngleRadians, endAngleRadians, stopAtEnd });
	}

	RotarySlider::RotaryParameters RotarySlider::getRotaryParameters() const noexcept { return rotaryParams; }

	void RotarySlider::setTextBoxStyle(TextEntryBoxPosition newPosition, bool isReadOnly, int textEntryBoxWidth, int textEntryBoxHeight) {
		if (textBoxPos != newPosition || editableText != (!isReadOnly) || textBoxWidth != textEntryBoxWidth || textBoxHeight != textEntryBoxHeight) {
			textBoxPos = newPosition;
			editableText = !isReadOnly;
			textBoxWidth = textEntryBoxWidth;
			textBoxHeight = textEntryBoxHeight;
			repaint();
			lookAndFeelChanged();
		}
	}

	RotarySlider::TextEntryBoxPosition RotarySlider::getTextBoxPosition() const noexcept { return textBoxPos; }

	int RotarySlider::getTextBoxWidth() const noexcept { return textBoxWidth; }

	int RotarySlider::getTextBoxHeight() const noexcept { return textBoxHeight; }

	void RotarySlider::setTextBoxIsEditable(bool shouldBeEditable) {
		editableText = shouldBeEditable;
		updateTextBoxEnablement();
	}

	bool RotarySlider::isTextBoxEditable() const noexcept { return editableText; }

	void RotarySlider::showTextBox() {
		jassert(editableText);
		if (valueBox != nullptr)
			valueBox->showEditor();
	}

	void RotarySlider::hideTextBox(bool discardCurrentEditorContents) {
		if (valueBox != nullptr) {
			valueBox->hideEditor(discardCurrentEditorContents);
			if (discardCurrentEditorContents)
				updateText();
		}
	}

	void RotarySlider::setValue(double newValue, juce::NotificationType notification = juce::sendNotificationAsync) {
		newValue = constrainedValue(newValue);
		if (newValue != lastCurrentValue) {
			if (valueBox != nullptr) {
				valueBox->hideEditor(true);
				lastCurrentValue = newValue;
				if (currentValue != newValue)
					currentValue = newValue;
				updateText();
				repaint();
				triggerChangeMessage(notification);
			}
		}
	}

	double RotarySlider::getValue() const { return currentValue.getValue(); }

	double RotarySlider::snapValue(double attemptedValue, RotarySlider::DragMode) {
		return attemptedValue;
	}

	void RotarySlider::setRange(double newMinimum,double newMaximum,double newInterval = 0) {
		normRange = juce::NormalisableRange<double>(newMinimum, newMaximum, newInterval);
		updateRange();
	}

	void RotarySlider::setRange(juce::Range<double> newRange, double newInterval) {
		setRange(newRange.getStart(), newRange.getEnd(), newInterval);
	}

	void RotarySlider::setNormalisableRange(juce::NormalisableRange<double> newNormalisableRange) {
		normRange = newNormalisableRange;
		updateRange();
	}

	juce::Range<double> RotarySlider::getRange() const noexcept { return { normRange.start, normRange.end }; }

	double RotarySlider::getMaximum() const noexcept { return normRange.end; }

	double RotarySlider::getMinimum() const noexcept { return normRange.start; }

	double RotarySlider::getInterval() const noexcept { return normRange.interval; }

	void RotarySlider::addListener(RotarySlider::Listener* listener) { listeners.add(listener); }

	void RotarySlider::removeListener(RotarySlider::Listener* listener) { listeners.remove(listener); }

	void RotarySlider::setTextValueSuffix(const juce::String& suffix) {
		if (suffix != textSuffix) {
			textSuffix = suffix;
			updateText();
		}
	}

	juce::String RotarySlider::getTextValueSuffix() const { return textSuffix; }

	double RotarySlider::getValueFromText(const juce::String& text) {
		auto t = text.trimStart();
		if (t.endsWith(textSuffix)) {
			t = t.substring(0, t.length() - textSuffix.length());
		}
		if (valueFromTextFunction != nullptr) return valueFromTextFunction(t);
		while (t.startsWithChar('+'))
			t = t.substring(1).trimStart();
		return t.initialSectionContainingOnly("0123456789.,-").getDoubleValue();
	}

	juce::String RotarySlider::getTextFromValue(double value) {
		auto getText = [this](double val) {
			if (textFromValueFunction != nullptr) return textFromValueFunction(val);
			if (numDecimalPlaces > 0) return juce::String(val, numDecimalPlaces);
			return juce::String(juce::roundToInt(val));
		};
		return getText(value) + textSuffix;
	}

	void RotarySlider::setScrollWheelEnabled(bool enabled) { scrollWheelEnabled = enabled; }

	double RotarySlider::proportionOfLengthToValue(double proportion) {
		return normRange.convertFrom0to1(proportion);
	}

	double RotarySlider::valueToProportionOfLength(double value) {
		return normRange.convertTo0to1(value);
	}

	void RotarySlider::restoreMouseIfHidden() {
		for (auto& ms : juce::Desktop::getInstance().getMouseSources())
		{
			if (ms.isUnboundedMouseMovementEnabled())
			{
				ms.enableUnboundedMouseMovement(false);

				auto pos = static_cast<double> (currentValue.getValue());
				juce::Point<float> mousePos;

				mousePos = ms.getLastMouseDownPosition();

				auto delta = (float)(pixelsForFullDragExtent * (valueToProportionOfLength(valueOnMouseDown)
					- valueToProportionOfLength(pos)));

				mousePos += juce::Point<float>(delta / -2.0f, delta / 2.0f);
				mousePos = getScreenBounds().reduced(4).toFloat().getConstrainedPoint(mousePos);
				mouseDragStartPos = mousePosWhenLastDragged = getLocalPoint(nullptr, mousePos);
				valueOnMouseDown = valueWhenLastDragged;
				const_cast <juce::MouseInputSource&> (ms).setScreenPosition(mousePos);
			}
		}
	}

	void RotarySlider::triggerChangeMessage(juce::NotificationType notification) {
		if (notification != juce::dontSendNotification) {
			valueChanged();
			if (notification == juce::sendNotificationSync)
				handleAsyncUpdate();
			else
				triggerAsyncUpdate();
		}
	}

	void RotarySlider::paint(juce::Graphics& g) {
		auto sliderPos = (float)valueToProportionOfLength(lastCurrentValue);
		jassert(sliderPos >= 0 && sliderPos <= 1.0f);
		// TODO: change to use RotarySlider::LookAndFeelMethods::drawRotarySlider()
		drawRotarySlider(g, sliderPos);
	}

	void RotarySlider::resized() {
		getSliderLayout();
		// TODO: can get rid of sliderRect or sliderBounds (I think...)
		sliderRect = sliderBounds;
		if (valueBox != nullptr) {
			valueBox->setBounds(textBoxBounds);
		}
	}

	void RotarySlider::mouseDown(const juce::MouseEvent& e) {
		useDragEvents = false;
		mouseDragStartPos = mousePosWhenLastDragged = e.position;
		currentDrag.reset();
		if (isEnabled()) {
			if (normRange.end > normRange.start) {
				useDragEvents = true;
				if (valueBox != nullptr)
					valueBox->hideEditor(true);
				lastAngle = rotaryParams.startAngleRadians + (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians) * valueToProportionOfLength(currentValue.getValue());
				valueWhenLastDragged = currentValue.getValue();
				valueOnMouseDown = valueWhenLastDragged;
				currentDrag.reset(new DragInProgress(*this));
				mouseDrag(e);
			}
		}
	}

	void RotarySlider::mouseUp(const juce::MouseEvent& e) {
		if (isEnabled() && useDragEvents && (normRange.end > normRange.start)) {
			restoreMouseIfHidden();

			//if (sendChangeOnlyOnRelease && valueOnMouseDown != static_cast<double> (currentValue.getValue())) {
			//	triggerChangeMessage(juce::sendNotificationAsync);
			//}
		}
		currentDrag.reset();
	}

	void RotarySlider::mouseDrag(const juce::MouseEvent& e) {
		if (isEnabled() && useDragEvents && normRange.end > normRange.start && !(e.mouseWasClicked() && valueBox != nullptr && valueBox->isEditable())) {
			DragMode dragMode = notDragging;
			handleRotaryDrag(e);
			valueWhenLastDragged = juce::jlimit(normRange.start, normRange.end, valueWhenLastDragged);

			setValue(snapValue(valueWhenLastDragged, dragMode), juce::sendNotification);

			mousePosWhenLastDragged = e.position;
		}
	}

	void RotarySlider::handleAsyncUpdate() {
		cancelPendingUpdate();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderValueChanged(this); });
		if (checker.shouldBailOut()) return;
		if (onValueChange != nullptr) onValueChange();
	}

	// should override in derived classes
	void RotarySlider::valueChanged(juce::Value& value) {
		if (value.refersToSameSourceAs(currentValue)) {
			setValue(currentValue.getValue(), juce::dontSendNotification);
		}
	}

	void RotarySlider::updateTextBoxEnablement() {
		if (valueBox != nullptr) {
			bool shouldBeEditable = editableText && isEnabled();
			if (valueBox->isEditable() != shouldBeEditable)
				valueBox->setEditable(shouldBeEditable);
		}
	}

	// TODO
	void RotarySlider::mouseDoubleClick(const juce::MouseEvent& e) {}

	double RotarySlider::getMouseWheelData(double value, double wheelAmount) {
		auto proportionDelta = wheelAmount * 0.15;
		auto currentPos = valueToProportionOfLength(value);
		auto newPos = currentPos + proportionDelta;
		newPos = rotaryParams.stopAtEnd ? newPos - std::floor(newPos) : juce::jlimit(0.0, 1.0, newPos);
		return proportionOfLengthToValue(newPos) - value;
	}

	void RotarySlider::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) {
		if (scrollWheelEnabled) {
			if (e.eventTime != lastMouseWheelTime) {
				lastMouseWheelTime = e.eventTime;
				if (normRange.end > normRange.start && !e.mods.isAnyMouseButtonDown()) {
					if (valueBox != nullptr)
						valueBox->hideEditor(false);
					auto value = static_cast<double>(currentValue.getValue());
					auto delta = getMouseWheelData(value, (std::abs(wheel.deltaX) > std::abs(wheel.deltaY) ? -wheel.deltaX : wheel.deltaY) * (wheel.isReversed ? -1.0f : 1.0f));
					if (delta != 0.0) {
						auto newValue = value + juce::jmax(normRange.interval, std::abs(delta)) * (delta < 0 ? -1.0 : 1.0);
						DragInProgress drag(*this);
						setValue(snapValue(newValue, notDragging), juce::sendNotificationSync);
					}
				}
			}
			if (!isEnabled())
				Component::mouseWheelMove(e, wheel);
		}
	}

	// TODO
	void RotarySlider::modifierKeysChanged(const juce::ModifierKeys&) {}

	void RotarySlider::lookAndFeelChanged() {
		if (textBoxPos != NoTextBox) {
			auto previousTextBoxContent = (valueBox != nullptr ? valueBox->getText() : getTextFromValue(currentValue.getValue()));
			valueBox.reset();

			valueBox.reset(createRotarySliderTextBox());

			addAndMakeVisible(valueBox.get());
			valueBox->setWantsKeyboardFocus(false);
			valueBox->setText(previousTextBoxContent, juce::dontSendNotification);
			//valueBox->setTooltip(getToolTip());
			updateTextBoxEnablement();
			valueBox->onTextChange = [this] {textChanged(); };
		}
		else {
			valueBox.reset();
		}
		//setComponentEffect();

		resized();
		repaint();
	}

	void RotarySlider::enablementChanged() {
		repaint();
		updateTextBoxEnablement();
	}

	// TODO
	void RotarySlider::focusOfChildComponentChanged(RotarySlider::FocusChangeType) {}

	void RotarySlider::colourChanged() {
		lookAndFeelChanged();
	}

	// TODO
	void RotarySlider::mouseMove(const juce::MouseEvent& e) {}

	// TODO
	void RotarySlider::mouseExit(const juce::MouseEvent& e) {}

	// TODO
	void RotarySlider::mouseEnter(const juce::MouseEvent& e) {}

	void RotarySlider::textChanged() {
		auto newValue = snapValue(getValueFromText(valueBox->getText()), notDragging);
		if (newValue != static_cast<double>(currentValue.getValue())) {
			DragInProgress drag(*this);
			setValue(newValue, juce::dontSendNotification);
		}
		updateText();
	}

	void RotarySlider::updateText() {
		if (valueBox != nullptr) {
			auto newValue = getTextFromValue(currentValue.getValue());
			if (newValue != valueBox->getText())
				valueBox->setText(newValue, juce::dontSendNotification);
		}
	}

	void RotarySlider::updateRange() {
		numDecimalPlaces = 7;
		if (normRange.interval != 0.0) {
			int v = std::abs(juce::roundToInt(normRange.interval * 10000000));
			while ((v % 10) == 0 && numDecimalPlaces > 0)
			{
				--numDecimalPlaces;
				v /= 10;
			}
		}
		setValue(getValue(), juce::dontSendNotification);
		updateText();
	}

	double RotarySlider::constrainedValue(double value) const { return normRange.snapToLegalValue(value); }

	void RotarySlider::getSliderLayout() {
		int minXSpace = 0;
		int minYSpace = 0;

		if (textBoxPos == TextBoxLeft || textBoxPos == TextBoxRight)
			minXSpace = 30;
		else
			minYSpace = 15;

		auto localBounds = getLocalBounds();
		auto newTextBoxWidth = juce::jmax(0, juce::jmin(textBoxWidth, localBounds.getWidth() - minXSpace));
		auto newTextBoxHeight = juce::jmax(0, juce::jmin(textBoxHeight, localBounds.getHeight() - minYSpace));

		if (textBoxPos != RotarySlider::NoTextBox) {
			textBoxBounds.setWidth(textBoxWidth);
			textBoxBounds.setHeight(textBoxHeight);
			if (textBoxPos == RotarySlider::TextBoxLeft) textBoxBounds.setX(0);
			else if (textBoxPos == RotarySlider::TextBoxRight) textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
			else textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

			if (textBoxPos == RotarySlider::TextBoxAbove) textBoxBounds.setY(0);
			else if (textBoxPos == RotarySlider::TextBoxBelow) textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
			else textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
		}

		sliderBounds = localBounds;
		if (textBoxPos == RotarySlider::TextBoxLeft) sliderBounds.removeFromLeft(textBoxWidth);
		else if (textBoxPos == RotarySlider::TextBoxRight) sliderBounds.removeFromRight(textBoxWidth);
		else if (textBoxPos == RotarySlider::TextBoxAbove) sliderBounds.removeFromTop(textBoxHeight);
		else if (textBoxPos == RotarySlider::TextBoxBelow) sliderBounds.removeFromBottom(textBoxHeight);
	}

	juce::Label* RotarySlider::createRotarySliderTextBox() {
		auto l = new juce::Label();
		l->setJustificationType(juce::Justification::centred);
		l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
		l->setColour(juce::Label::textColourId, findColour(textBoxTextColourId));
		l->setColour(juce::Label::backgroundColourId, findColour(textBoxBackgroundColourId));
		l->setColour(juce::Label::outlineColourId, findColour(textBoxBackgroundColourId));//textBoxOutlineColourId));
		l->setColour(juce::TextEditor::textColourId, findColour(textBoxTextColourId));
		l->setColour(juce::TextEditor::backgroundColourId, findColour(textBoxBackgroundColourId));
		l->setColour(juce::TextEditor::outlineColourId, findColour(textBoxOutlineColourId));
		l->setColour(juce::TextEditor::highlightColourId, findColour(textBoxHighlightColourId));
		return l;
	}

	void RotarySlider::drawRotarySlider(juce::Graphics& g, float sliderPos) {
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
	}

	void RotarySlider::sendDragStart() {
		startedDragging();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderDragStarted(this); });
		if (checker.shouldBailOut())
			return;
		if (onDragStart != nullptr) onDragStart();
	}

	void RotarySlider::sendDragEnd() {
		stoppedDragging();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderDragEnded(this); });
		if (checker.shouldBailOut()) return;
		if (onDragEnd != nullptr) onDragEnd();
	}

	double RotarySlider::smallestAngleBetween(double a1, double a2) noexcept {
		return juce::jmin(std::abs(a1 - a2),
			std::abs(a1 + juce::MathConstants<double>::twoPi - a2),
			std::abs(a2 + juce::MathConstants<double>::twoPi - a1));
	}

	void RotarySlider::handleRotaryDrag(const juce::MouseEvent& e) {
		auto dx = e.position.x - (float)sliderRect.getCentreX();
		auto dy = e.position.y - (float)sliderRect.getCentreY();
		if (dx * dx + dy * dy > 25.0f) {
			auto angle = std::atan2((double)dx, (double)-dy);
			while (angle < 0.0) {
				angle += juce::MathConstants<double>::twoPi;
			}
			angle = limitAngleForRotaryDrag(e, angle, rotaryParams.startAngleRadians, rotaryParams.endAngleRadians);

			auto proportion = (angle - rotaryParams.startAngleRadians) / (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
			valueWhenLastDragged = proportionOfLengthToValue(juce::jlimit(0.0, 1.0, proportion));
			lastAngle = angle;
		}
	}

	double RotarySlider::limitAngleForRotaryDrag(const juce::MouseEvent& e, double angle, float minLegalAngle, float maxLegalAngle) {
		if (rotaryParams.stopAtEnd && e.mouseWasDraggedSinceMouseDown()) {
			if (std::abs(angle - lastAngle) > juce::MathConstants<double>::pi) {
				if (angle >= lastAngle) {
					angle -= juce::MathConstants<double>::twoPi;
				}
				else {
					angle += juce::MathConstants<double>::twoPi;
				}
			}
			if (angle >= lastAngle) {
				angle = juce::jmin(angle, (double)juce::jmax(minLegalAngle, maxLegalAngle));
			}
			else {
				angle = juce::jmax(angle, (double)juce::jmin(minLegalAngle, maxLegalAngle));
			}
		}
		else {
			while (angle < minLegalAngle) {
				angle += juce::MathConstants<double>::twoPi;
			}
			if (angle > maxLegalAngle) {
				if (smallestAngleBetween(angle, minLegalAngle) <= smallestAngleBetween(angle, maxLegalAngle)) {
					angle = minLegalAngle;
				}
				else {
					angle = maxLegalAngle;
				}
			}
		}
		return angle;
	}

	//=========================== PAN ROTARY SLIDER ===========================================//
	PanRotarySlider::PanRotarySlider() {
		setRange(-PAN_MAX_MAGNITUDE, PAN_MAX_MAGNITUDE, 1.f);
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
					auto halfWidth = 0.5f * ((float)maxValue.getValue() - (float)minValue.getValue());
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

	// TODO: move this to PanRotarySlider::LookAndFeelMethods::drawRotarySlider()
	void PanRotarySlider::drawRotarySlider(juce::Graphics& g, float sliderPos) {
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
		auto halfWidth = 0.5 * slider->getValue();
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
			float angleRangeRadians = rotaryParams.endAngleRadians - rotaryParams.startAngleRadians;

			if (hasExternalWidthController && (maxValue.getValue() != minValue.getValue())) {
				auto halfWidth = (float)maxValue.getValue() - (float)currentValue.getValue();
				//auto halfWidthAngle = rotaryParams.startAngleRadians + angleRangeRadians * valueToProportionOfLength(halfWidth);
				minLegalAngle = rotaryParams.startAngleRadians + angleRangeRadians * valueToProportionOfLength((float)-PAN_MAX_MAGNITUDE + halfWidth);
				maxLegalAngle = rotaryParams.startAngleRadians + angleRangeRadians * valueToProportionOfLength((float)PAN_MAX_MAGNITUDE - halfWidth);
			}

			angle = limitAngleForRotaryDrag(e, angle, minLegalAngle, maxLegalAngle);

			auto proportion = (angle - rotaryParams.startAngleRadians) / angleRangeRadians;
			valueWhenLastDragged = proportionOfLengthToValue(juce::jlimit(0.0, 1.0, proportion));
			lastAngle = angle;
		}
	}

};