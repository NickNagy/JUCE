#ifndef ROTARY_SLIDERS_H
#define ROTARY_SLIDERS_H

#include <JuceHeader.h>

/* these classes are based on the juce::Slider class, but are customized to an extent that inheriting from juce::Slider is not sufficient */
/* parts of the code are taken from the juce::Slider class directly */

class RotarySlider : public juce::Component, private juce::Value::Listener, public juce::AsyncUpdater {
public:
	enum TextEntryBoxPosition
	{
		NoTextBox,              /**< Doesn't display a text box.  */
		TextBoxLeft,            /**< Puts the text box to the left of the slider, vertically centred.  */
		TextBoxRight,           /**< Puts the text box to the right of the slider, vertically centred.  */
		TextBoxAbove,           /**< Puts the text box above the slider, horizontally centred.  */
		TextBoxBelow            /**< Puts the text box below the slider, horizontally centred.  */
	};

	enum DragMode
	{
		notDragging,            /**< Dragging is not active.  */
		absoluteDrag,           /**< The dragging corresponds directly to the value that is displayed.  */
		velocityDrag            /**< The dragging value change is relative to the velocity of the mouse movement.  */
	};

	/** Structure defining rotary parameters for a slider */
	struct RotaryParameters
	{
		/** The angle (in radians, clockwise from the top) at which
			the slider's minimum value is represented.
		*/
		float startAngleRadians;

		/** The angle (in radians, clockwise from the top) at which
			the slider's maximum value is represented. This must be
			greater than startAngleRadians.
		*/
		float endAngleRadians;

		/** Determines what happens when a circular drag action rotates beyond
			the minimum or maximum angle. If true, the value will stop changing
			until the mouse moves back the way it came; if false, the value
			will snap back to the value nearest to the mouse. Note that this has
			no effect if the drag mode is vertical or horizontal.
		*/
		bool stopAtEnd;
	};

	RotarySlider() : textBoxPos(TextEntryBoxPosition::TextBoxBelow) {
		setWantsKeyboardFocus(false);
		setRepaintsOnMouseActivity(true);
		rotaryParams.startAngleRadians = juce::MathConstants<float>::pi * 1.2f;
		rotaryParams.endAngleRadians = juce::MathConstants<float>::pi * 2.8f;
		rotaryParams.stopAtEnd = true;
		lookAndFeelChanged();
		updateText();
		currentValue.addListener(this);
	}

	~RotarySlider() override {
		currentValue.removeListener(this);
	};

	/** Changes the properties of a rotary slider. */
	void setRotaryParameters(RotaryParameters newParameters) noexcept {
		jassert(newParameters.startAngleRadians >= 0 && newParameters.endAngleRadians >= 0);
		jassert(newParameters.startAngleRadians < juce::MathConstants<float>::pi * 4.0f && newParameters.endAngleRadians < juce::MathConstants<float>::pi * 4.0f);
		rotaryParams = newParameters;
	}

	/** Changes the properties of a rotary slider. */
	void setRotaryParameters(float startAngleRadians,
		float endAngleRadians,
		bool stopAtEnd) noexcept {
		setRotaryParameters({ startAngleRadians, endAngleRadians, stopAtEnd });
	}

	/** Changes the properties of a rotary slider. */
	RotaryParameters getRotaryParameters() const noexcept { return rotaryParams; }

	//==============================================================================
/** Changes the location and properties of the text-entry box.

	@param newPosition          where it should go (or NoTextBox to not have one at all)
	@param isReadOnly           if true, it's a read-only display
	@param textEntryBoxWidth    the width of the text-box in pixels. Make sure this leaves enough
								room for the slider as well!
	@param textEntryBoxHeight   the height of the text-box in pixels. Make sure this leaves enough
								room for the slider as well!

	@see setTextBoxIsEditable, getValueFromText, getTextFromValue
*/
	void setTextBoxStyle(TextEntryBoxPosition newPosition,
		bool isReadOnly,
		int textEntryBoxWidth,
		int textEntryBoxHeight) {
		if (textBoxPos != newPosition || editableText != (!isReadOnly) || textBoxWidth != textEntryBoxWidth || textBoxHeight != textEntryBoxHeight) {
			textBoxPos = newPosition;
			editableText = !isReadOnly;
			textBoxWidth = textEntryBoxWidth;
			textBoxHeight = textEntryBoxHeight;
			repaint();
			lookAndFeelChanged();
		}
	}

	/** Returns the status of the text-box.
		@see setTextBoxStyle
	*/
	TextEntryBoxPosition getTextBoxPosition() const noexcept { return textBoxPos; }

	/** Returns the width used for the text-box.
		@see setTextBoxStyle
	*/
	int getTextBoxWidth() const noexcept { return textBoxWidth; }

	/** Returns the height used for the text-box.
		@see setTextBoxStyle
	*/
	int getTextBoxHeight() const noexcept { return textBoxHeight; }

	/** Makes the text-box editable.

		By default this is true, and the user can enter values into the textbox,
		but it can be turned off if that's not suitable.

		@see setTextBoxStyle, getValueFromText, getTextFromValue
	*/
	void setTextBoxIsEditable(bool shouldBeEditable) { 
		editableText = shouldBeEditable; 
		updateTextBoxEnablement();
	};

	/** Returns true if the text-box is read-only.
		@see setTextBoxStyle
	*/
	bool isTextBoxEditable() const noexcept {
		return editableText;
	}

	/** If the text-box is editable, this will give it the focus so that the user can
		type directly into it.
		This is basically the effect as the user clicking on it.
	*/
	void showTextBox() {
		jassert(editableText);
		if (valueBox != nullptr)
			valueBox->showEditor();
	}

	/** If the text-box currently has focus and is being edited, this resets it and takes keyboard
		focus away from it.

		@param discardCurrentEditorContents     if true, the slider's value will be left
												unchanged; if false, the current contents of the
												text editor will be used to set the slider position
												before it is hidden.
	*/
	void hideTextBox(bool discardCurrentEditorContents) {
		if (valueBox != nullptr) {
			valueBox->hideEditor(discardCurrentEditorContents);
			if (discardCurrentEditorContents)
				updateText();
		}
	}

	//==============================================================================
    /** Changes the slider's current value.

        This will trigger a callback to Slider::Listener::sliderValueChanged() for any listeners
        that are registered, and will synchronously call the valueChanged() method in case subclasses
        want to handle it.

        @param newValue         the new value to set - this will be restricted by the
                                minimum and maximum range, and will be snapped to the
                                nearest interval if one has been set
        @param notification     can be one of the NotificationType values, to request
                                a synchronous or asynchronous call to the valueChanged() method
                                of any Slider::Listeners that are registered. A notification will
                                only be sent if the Slider's value has changed.
    */
	void setValue(double newValue, juce::NotificationType notification = juce::sendNotificationAsync) {
		newValue = constrainedValue(newValue);
		if (newValue != lastCurrentValue) {
			if (valueBox != nullptr) {
				valueBox->hideEditor(true);
				lastCurrentValue = newValue;
				if (currentValue != newValue)
					currentValue = newValue;
				updateText();
				repaint();
			}
		}
	}

    /** Returns the slider's current value. */
	double getValue() const { return currentValue.getValue(); }

	double snapValue(double attemptedValue, DragMode) {
		return attemptedValue;
	}

	//==============================================================================
	/** Sets the limits that the slider's value can take.

		@param newMinimum   the lowest value allowed
		@param newMaximum   the highest value allowed
		@param newInterval  the steps in which the value is allowed to increase - if this
							is not zero, the value will always be (newMinimum + (newInterval * an integer)).
	*/
	void setRange(double newMinimum,
		double newMaximum,
		double newInterval = 0) {
		normRange = juce::NormalisableRange<double>(newMinimum, newMaximum, newInterval);
		updateRange();
	}

	/** Sets the limits that the slider's value can take.

		@param newRange     the range to allow
		@param newInterval  the steps in which the value is allowed to increase - if this
							is not zero, the value will always be (newMinimum + (newInterval * an integer)).
	*/
	void setRange(juce::Range<double> newRange, double newInterval) {
		setRange(newRange.getStart(), newRange.getEnd(), newInterval);
	}

	/** Sets a NormalisableRange to use for the Slider values.

		@param newNormalisableRange     the NormalisableRange to use
	*/
	void setNormalisableRange(juce::NormalisableRange<double> newNormalisableRange) {
		normRange = newNormalisableRange;
		updateRange();
	}

	/** Returns the slider's range. */
	juce::Range<double> getRange() const noexcept { return {normRange.start, normRange.end}; }

	/** Returns the current maximum value.
		@see setRange, getRange
	*/
	double getMaximum() const noexcept { return normRange.end; }

	/** Returns the current minimum value.
		@see setRange, getRange
	*/
	double getMinimum() const noexcept { return normRange.start;}

	/** Returns the current step-size for values.
		@see setRange, getRange
	*/
	double getInterval() const noexcept { return normRange.interval; }

	//==============================================================================
	/** A class for receiving callbacks from a Slider.

		To be told when a slider's value changes, you can register a Slider::Listener
		object using Slider::addListener().

		@see Slider::addListener, Slider::removeListener
	*/
	class JUCE_API  Listener
	{
	public:
		//==============================================================================
		/** Destructor. */
		virtual ~Listener() = default;

		//==============================================================================
		/** Called when the slider's value is changed.

			This may be caused by dragging it, or by typing in its text entry box,
			or by a call to Slider::setValue().

			You can find out the new value using Slider::getValue().

			@see Slider::valueChanged
		*/
		virtual void sliderValueChanged(RotarySlider* slider) = 0;

		//==============================================================================
		/** Called when the slider is about to be dragged.

			This is called when a drag begins, then it's followed by multiple calls
			to sliderValueChanged(), and then sliderDragEnded() is called after the
			user lets go.

			@see sliderDragEnded, Slider::startedDragging
		*/
		virtual void sliderDragStarted(RotarySlider*) {}

		/** Called after a drag operation has finished.
			@see sliderDragStarted, Slider::stoppedDragging
		*/
		virtual void sliderDragEnded(RotarySlider*) {}
	};

	/** Adds a listener to be called when this slider's value changes. */
	void addListener(Listener* listener) { listeners.add(listener); }

	/** Removes a previously-registered listener. */
	void removeListener(Listener* listener) { listeners.remove(listener); }

	//==============================================================================
	/** You can assign a lambda to this callback object to have it called when the slider value is changed. */
	std::function<void()> onValueChange;

	/** You can assign a lambda to this callback object to have it called when the slider's drag begins. */
	std::function<void()> onDragStart;

	/** You can assign a lambda to this callback object to have it called when the slider's drag ends. */
	std::function<void()> onDragEnd;

	/** You can assign a lambda that will be used to convert textual values to the slider's normalised position. */
	std::function<double(const juce::String&)> valueFromTextFunction;

	/** You can assign a lambda that will be used to convert the slider's normalised position to a textual value. */
	std::function<juce::String(double)> textFromValueFunction;

	//==============================================================================
/** Subclasses can override this to convert a text string to a value.

	When the user enters something into the text-entry box, this method is
	called to convert it to a value.
	The default implementation just tries to convert it to a double.

	@see getTextFromValue
*/
	juce::String getTextValueSuffix() const { return textSuffix; }

	virtual double getValueFromText(const juce::String& text) {
		auto t = text.trimStart();
		if (t.endsWith(textSuffix)) {
			t = t.substring(0, t.length() - textSuffix.length());
		}
		if (valueFromTextFunction != nullptr) return valueFromTextFunction(t);
		while (t.startsWithChar('+'))
			t = t.substring(1).trimStart();
		return t.initialSectionContainingOnly("0123456789.,-").getDoubleValue();
	}

	/** Turns the slider's current value into a text string.

		Subclasses can override this to customise the formatting of the text-entry box.

		The default implementation just turns the value into a string, using
		a number of decimal places based on the range interval. If a suffix string
		has been set using setTextValueSuffix(), this will be appended to the text.

		@see getValueFromText
	*/
	virtual juce::String getTextFromValue(double value) {
		auto getText = [this](double val) {
			if (textFromValueFunction != nullptr) return textFromValueFunction(val);
			if (numDecimalPlaces > 0) return juce::String(val, numDecimalPlaces);
			return juce::String(juce::roundToInt(val));
		};
		return getText(value) + textSuffix;
	}

	/** This can be used to stop the mouse scroll-wheel from moving the slider.
	By default it's enabled.
	*/
	void setScrollWheelEnabled(bool enabled) { scrollWheelEnabled = enabled; }

	//==============================================================================
	/** Callback to indicate that the user is about to start dragging the slider.
	@see Slider::Listener::sliderDragStarted
	*/
	virtual void startedDragging() {}

	/** Callback to indicate that the user has just stopped dragging the slider.
		@see Slider::Listener::sliderDragEnded
	*/
	virtual void stoppedDragging() {}

	/** Callback to indicate that the user has just moved the slider.
		@see Slider::Listener::sliderValueChanged
	*/
	// not to be confused with override of Value::Listener::valueChanged(Value& value)
	virtual void valueChanged() {}

	//==============================================================================
	/** Allows a user-defined mapping of distance along the slider to its value.

	The default implementation for this performs the skewing operation that
	can be set up in the setSkewFactor() method. Override it if you need
	some kind of custom mapping instead, but make sure you also implement the
	inverse function in valueToProportionOfLength().

	@param proportion       a value 0 to 1.0, indicating a distance along the slider
	@returns                the slider value that is represented by this position
	@see valueToProportionOfLength
	*/
	virtual double proportionOfLengthToValue(double proportion) {
		return normRange.convertFrom0to1(proportion);
	}

	/** Allows a user-defined mapping of value to the position of the slider along its length.

		The default implementation for this performs the skewing operation that
		can be set up in the setSkewFactor() method. Override it if you need
		some kind of custom mapping instead, but make sure you also implement the
		inverse function in proportionOfLengthToValue().

		@param value            a valid slider value, between the range of values specified in
								setRange()
		@returns                a value 0 to 1.0 indicating the distance along the slider that
								represents this value
		@see proportionOfLengthToValue
	*/
	virtual double valueToProportionOfLength(double value) {
		return normRange.convertTo0to1(value);
	}

	//==============================================================================
	/** A set of colour IDs to use to change the colour of various aspects of the slider.

	These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
	methods.

	@see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
	*/
	enum ColourIds
	{
		backgroundColourId = 0x1001200,  /**< A colour to use to fill the slider's background. */
		thumbColourId = 0x1001300,  /**< The colour to draw the thumb with. It's up to the look and feel class how this is used. */
		trackColourId = 0x1001310,  /**< The colour to draw the groove that the thumb moves along. */
		rotarySliderFillColourId = 0x1001311,  /**< For rotary sliders, this colour fills the outer curve. */
		rotarySliderOutlineColourId = 0x1001312,  /**< For rotary sliders, this colour is used to draw the outer curve's outline. */
		textBoxTextColourId = 0x1001400,  /**< The colour for the text in the text-editor box used for editing the value. */
		textBoxBackgroundColourId = 0x1001500,  /**< The background colour for the text-editor box. */
		textBoxHighlightColourId = 0x1001600,  /**< The text highlight colour for the text-editor box. */
		textBoxOutlineColourId = 0x1001700   /**< The colour to use for a border around the text-editor box. */
	};

	//==============================================================================
	/** A struct defining the placement of the slider area and the text box area
	relative to the bounds of the whole Slider component.
	*/
	struct RotarySliderLayout
	{
		juce::Rectangle<int> sliderBounds;
		juce::Rectangle<int> textBoxBounds;
	};

	//==============================================================================
	/** This abstract base class is implemented by LookAndFeel classes to provide
	slider drawing functionality.
	*/
	struct JUCE_API  LookAndFeelMethods
	{
		virtual ~LookAndFeelMethods() = default;

		//==============================================================================
		virtual int getSliderThumbRadius(RotarySlider&) = 0;

		virtual void drawRotarySlider(juce::Graphics&,
			int x, int y, int width, int height,
			float sliderPosProportional,
			float rotaryStartAngle,
			float rotaryEndAngle,
			RotarySlider&) = 0;

		virtual juce::Button* createSliderButton(RotarySlider&, bool isIncrement) = 0;
		virtual juce::Label* createRotarySliderTextBox(RotarySlider&) {} //= 0;

		virtual juce::ImageEffectFilter* getSliderEffect(RotarySlider&) = 0;

		virtual RotarySliderLayout getSliderLayout(RotarySlider&) = 0;
	};

	void restoreMouseIfHidden() {
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

	//==============================================================================
	/** @internal */
	void paint(juce::Graphics& g) override {
		auto sliderPos = (float)valueToProportionOfLength(lastCurrentValue);
		jassert(sliderPos >= 0 && sliderPos <= 1.0f);
		
		// TODO: can't use drawRotarySlider from lookAndFeel because it expects a slider object
		drawRotarySlider(g, sliderPos);
	}

	/** @internal */
	void resized() override {
		getSliderLayout();
		// TODO: can get rid of sliderRect or sliderBounds (I think...)
		sliderRect = sliderBounds;
		if (valueBox != nullptr) {
			valueBox->setBounds(textBoxBounds);
		}
	}

	/** @internal */
	void mouseDown(const juce::MouseEvent& e) override {
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

	/** @internal */
	void mouseUp(const juce::MouseEvent&) override {
		if (isEnabled() && useDragEvents && (normRange.end > normRange.start)) {
			restoreMouseIfHidden();
			//if (sendChangeOnlyOnRelease && valueOnMouseDown != static_cast<double> (currentValue.getValue())) {
			//
			//}
		}
		currentDrag.reset();
	}

	/** @internal */
	void mouseDrag(const juce::MouseEvent& e) override {
		if (isEnabled() && useDragEvents && normRange.end > normRange.start && !(e.mouseWasClicked() && valueBox != nullptr && valueBox->isEditable())) {
			DragMode dragMode = notDragging;
			handleRotaryDrag(e);
			valueWhenLastDragged = juce::jlimit(normRange.start, normRange.end, valueWhenLastDragged);
			
			setValue(snapValue(valueWhenLastDragged, dragMode), juce::sendNotification);
			
			mousePosWhenLastDragged = e.position;
		}
	}

	void handleAsyncUpdate() override {
		cancelPendingUpdate();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderValueChanged(this); });
		if (checker.shouldBailOut()) return;
		if (onValueChange != nullptr) onValueChange();
	}

	void valueChanged(juce::Value& value) override {
		if (value.refersToSameSourceAs(currentValue)) {
			setValue(currentValue.getValue(), juce::dontSendNotification);
		}
	}

	void updateTextBoxEnablement() {
		if (valueBox != nullptr) {
			bool shouldBeEditable = editableText && isEnabled();
			if (valueBox->isEditable() != shouldBeEditable)
				valueBox->setEditable(shouldBeEditable);
		}
	}

	/** @internal */
	void mouseDoubleClick(const juce::MouseEvent&) override {} // TODO

	double getMouseWheelData(double value, double wheelAmount) {
		auto proportionDelta = wheelAmount * 0.15;
		auto currentPos = valueToProportionOfLength(value);
		auto newPos = currentPos + proportionDelta;
		newPos = rotaryParams.stopAtEnd ? newPos - std::floor(newPos) : juce::jlimit(0.0, 1.0, newPos);
		return proportionOfLengthToValue(newPos) - value;
	}

	/** @internal */
	void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override {
		if (scrollWheelEnabled) {
			if (e.eventTime != lastMouseWheelTime) {
				lastMouseWheelTime = e.eventTime;
				if (normRange.end > normRange.start && !e.mods.isAnyMouseButtonDown()) {
					if (valueBox != nullptr)
						valueBox->hideEditor(false);
					auto value = static_cast<double>(currentValue.getValue());
					auto delta = getMouseWheelData(value, (std::abs(wheel.deltaX) > std::abs(wheel.deltaY) ? -wheel.deltaX : wheel.deltaY) * (wheel.isReversed? -1.0f : 1.0f));
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
	/** @internal */
	void modifierKeysChanged(const juce::ModifierKeys&) override {} // TODO
	
	/** @internal */
	void lookAndFeelChanged() override {
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
	
	/** @internal */
	void enablementChanged() override {
		repaint();
		updateTextBoxEnablement();
	}
	/** @internal */
	void focusOfChildComponentChanged(FocusChangeType) override {} // TODO

	/** @internal */
	void colourChanged() override {
		lookAndFeelChanged();
	}
	
	/** @internal */
	void mouseMove(const juce::MouseEvent& e) override {}
	/** @internal */
	void mouseExit(const juce::MouseEvent&) override {}
	/** @internal */
	void mouseEnter(const juce::MouseEvent& e) override { mouseMove(e); }

protected:
	struct DragInProgress {
		DragInProgress(RotarySlider& s) : owner(s) { owner.sendDragStart(); }
		~DragInProgress() { owner.sendDragEnd(); }

		RotarySlider& owner;

		JUCE_DECLARE_NON_COPYABLE(DragInProgress)
	};

	juce::Rectangle<int>sliderBounds;
	juce::Rectangle<int>textBoxBounds;

	bool useDragEvents = false;
	bool editableText = true;
	bool scrollWheelEnabled = true;

	juce::Time lastMouseWheelTime;
	juce::String textSuffix;
	juce::ListenerList<RotarySlider::Listener> listeners;
	juce::Value currentValue;
	double lastCurrentValue = 0;
	int pixelsForFullDragExtent = 250;
	RotaryParameters rotaryParams;
	juce::NormalisableRange<double> normRange{ 0.0, 10.0 };
	double valueWhenLastDragged = 0, valueOnMouseDown = 0, lastAngle = 0;
	juce::Point<float> mouseDragStartPos, mousePosWhenLastDragged;
	TextEntryBoxPosition textBoxPos;
	int numDecimalPlaces = 7;
	int textBoxWidth = 80, textBoxHeight = 20;
	juce::Rectangle<int> sliderRect;

	std::unique_ptr<juce::Label> valueBox;
	std::unique_ptr<DragInProgress> currentDrag;

	void textChanged() {
		auto newValue = snapValue(getValueFromText(valueBox->getText()), notDragging);
		if (newValue != static_cast<double>(currentValue.getValue())) {
			DragInProgress drag(*this);
			setValue(newValue, juce::dontSendNotification);
		}
		updateText();
	}

	void updateText() {
		if (valueBox != nullptr) {
			auto newValue = getTextFromValue(currentValue.getValue());
			if (newValue != valueBox->getText())
				valueBox->setText(newValue, juce::dontSendNotification);
		}
	}

	void updateRange() {
		numDecimalPlaces = 7;
		if (normRange.interval != 0.0) { // TODO
		}
		setValue(getValue(), juce::dontSendNotification);
		updateText();
	}

	double constrainedValue(double value) const
	{
		return normRange.snapToLegalValue(value);
	}

	// based off LookAndFeel_V2
	void getSliderLayout() {
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

	// TODO
	juce::Label* createRotarySliderTextBox() {
		auto l = new juce::Label();
		l->setJustificationType(juce::Justification::centred);
		l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
		l->setColour(juce::Label::textColourId, findColour(textBoxTextColourId));
		l->setColour(juce::Label::backgroundColourId, findColour(textBoxBackgroundColourId));
		l->setColour(juce::Label::outlineColourId, findColour(textBoxOutlineColourId));
		l->setColour(juce::TextEditor::textColourId, findColour(textBoxTextColourId));
		l->setColour(juce::TextEditor::backgroundColourId, findColour(textBoxBackgroundColourId));
		l->setColour(juce::TextEditor::outlineColourId, findColour(textBoxOutlineColourId));
		l->setColour(juce::TextEditor::highlightColourId, findColour(textBoxHighlightColourId));
		return l;
	}

	virtual void drawRotarySlider(juce::Graphics& g, float sliderPos) {
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

	void sendDragStart() {
		startedDragging();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderDragStarted(this); });
		if (checker.shouldBailOut())
			return;
		if (onDragStart != nullptr) onDragStart();
	}

	void sendDragEnd() {
		stoppedDragging();
		juce::Component::BailOutChecker checker(this);
		listeners.callChecked(checker, [&](RotarySlider::Listener& l) {l.sliderDragEnded(this); });
		if (checker.shouldBailOut()) return;
		if (onDragEnd != nullptr) onDragEnd();
	}

	//==============================================================================
	static double smallestAngleBetween(double a1, double a2) noexcept
	{
		return juce::jmin(std::abs(a1 - a2),
			std::abs(a1 + juce::MathConstants<double>::twoPi - a2),
			std::abs(a2 + juce::MathConstants<double>::twoPi - a1));
	}

	virtual void handleRotaryDrag(const juce::MouseEvent& e) {
		auto dx = e.position.x - (float)sliderRect.getCentreX();
		auto dy = e.position.y - (float)sliderRect.getCentreY();
		if (dx * dx + dy * dy > 25.0f) {
			auto angle = std::atan2((double)dx, (double)-dy);
			while (angle < 0.0) {
				angle += juce::MathConstants<double>::twoPi;
			}
			if (rotaryParams.stopAtEnd && e.mouseWasDraggedSinceMouseDown())
			{
				if (std::abs(angle - lastAngle) > juce::MathConstants<double>::pi)
				{
					if (angle >= lastAngle)
						angle -= juce::MathConstants<double>::twoPi;
					else
						angle += juce::MathConstants<double>::twoPi;
				}

				if (angle >= lastAngle)
					angle = juce::jmin(angle, (double)juce::jmax(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians));
				else
					angle = juce::jmax(angle, (double)juce::jmin(rotaryParams.startAngleRadians, rotaryParams.endAngleRadians));
			}
			else
			{
				while (angle < rotaryParams.startAngleRadians)
					angle += juce::MathConstants<double>::twoPi;

				if (angle > rotaryParams.endAngleRadians)
				{
					if (smallestAngleBetween(angle, rotaryParams.startAngleRadians)
						<= smallestAngleBetween(angle, rotaryParams.endAngleRadians))
						angle = rotaryParams.startAngleRadians;
					else
						angle = rotaryParams.endAngleRadians;
				}
			}

			auto proportion = (angle - rotaryParams.startAngleRadians) / (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
			valueWhenLastDragged = proportionOfLengthToValue(juce::jlimit(0.0, 1.0, proportion));
			lastAngle = angle;
		}
	}
private:

};

class PanRotarySlider : public RotarySlider {
public:
	PanRotarySlider() {};
	~PanRotarySlider() override {};
};

#endif