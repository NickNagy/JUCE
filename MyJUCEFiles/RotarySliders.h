#ifndef ROTARY_SLIDERS_H
#define ROTARY_SLIDERS_H

#include <JuceHeader.h>

/* these classes are based on the juce::Slider class, but are customized to an extent that inheriting from juce::Slider is not sufficient */
/* parts of the code are taken from the juce::Slider class directly */

namespace magna {
	//============================================== BASE CLASS: RotarySlider =============================================================//
	class RotarySlider : public juce::Component, protected juce::Value::Listener, public juce::AsyncUpdater {
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

		RotarySlider();

		~RotarySlider() override;

		/** Changes the properties of a rotary slider. */
		void setRotaryParameters(RotaryParameters newParameters) noexcept;

		/** Changes the properties of a rotary slider. */
		void setRotaryParameters(float startAngleRadians,float endAngleRadians,bool stopAtEnd) noexcept;

		/** Changes the properties of a rotary slider. */
		RotaryParameters getRotaryParameters() const noexcept;

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
		void setTextBoxStyle(TextEntryBoxPosition newPosition,bool isReadOnly,int textEntryBoxWidth,int textEntryBoxHeight);

		/** Returns the status of the text-box.
			@see setTextBoxStyle
		*/
		TextEntryBoxPosition getTextBoxPosition() const noexcept;

		/** Returns the width used for the text-box.
			@see setTextBoxStyle
		*/
		int getTextBoxWidth() const noexcept;

		/** Returns the height used for the text-box.
			@see setTextBoxStyle
		*/
		int getTextBoxHeight() const noexcept;

		/** Makes the text-box editable.

			By default this is true, and the user can enter values into the textbox,
			but it can be turned off if that's not suitable.

			@see setTextBoxStyle, getValueFromText, getTextFromValue
		*/
		void setTextBoxIsEditable(bool shouldBeEditable);

		/** Returns true if the text-box is read-only.
			@see setTextBoxStyle
		*/
		bool isTextBoxEditable() const noexcept;

		/** If the text-box is editable, this will give it the focus so that the user can
			type directly into it.
			This is basically the effect as the user clicking on it.
		*/
		void showTextBox();

		/** If the text-box currently has focus and is being edited, this resets it and takes keyboard
			focus away from it.

			@param discardCurrentEditorContents     if true, the slider's value will be left
													unchanged; if false, the current contents of the
													text editor will be used to set the slider position
													before it is hidden.
		*/
		void hideTextBox(bool discardCurrentEditorContents);

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
		virtual void setValue(double newValue, juce::NotificationType notification);

		/** Returns the slider's current value. */
		double getValue() const;

		double snapValue(double attemptedValue, DragMode);

		//==============================================================================
		/** Sets the limits that the slider's value can take.

			@param newMinimum   the lowest value allowed
			@param newMaximum   the highest value allowed
			@param newInterval  the steps in which the value is allowed to increase - if this
								is not zero, the value will always be (newMinimum + (newInterval * an integer)).
		*/
		void setRange(double newMinimum,double newMaximum,double newInterval);

		/** Sets the limits that the slider's value can take.

		@param newRange     the range to allow
		@param newInterval  the steps in which the value is allowed to increase - if this
						is not zero, the value will always be (newMinimum + (newInterval * an integer)).
		*/
		void setRange(juce::Range<double> newRange, double newInterval);

		/** Sets a NormalisableRange to use for the Slider values.

			@param newNormalisableRange     the NormalisableRange to use
		*/
		void setNormalisableRange(juce::NormalisableRange<double> newNormalisableRange);

		/** Returns the slider's range. */
		juce::Range<double> getRange() const noexcept;

		/** Returns the current maximum value.
			@see setRange, getRange
		*/
		double getMaximum() const noexcept;

		/** Returns the current minimum value.
			@see setRange, getRange
		*/
		double getMinimum() const noexcept;

		/** Returns the current step-size for values.
			@see setRange, getRange
		*/
		double getInterval() const noexcept;

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
		void addListener(Listener* listener);

		/** Removes a previously-registered listener. */
		void removeListener(Listener* listener);

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
		void setTextValueSuffix(const juce::String& suffix);

		juce::String getTextValueSuffix() const;

		/** Subclasses can override this to convert a text string to a value.

		When the user enters something into the text-entry box, this method is
		called to convert it to a value.
		The default implementation just tries to convert it to a double.

		@see getTextFromValue
		*/
		virtual double getValueFromText(const juce::String& text);

		/** Turns the slider's current value into a text string.

			Subclasses can override this to customise the formatting of the text-entry box.

			The default implementation just turns the value into a string, using
			a number of decimal places based on the range interval. If a suffix string
			has been set using setTextValueSuffix(), this will be appended to the text.

			@see getValueFromText
		*/
		virtual juce::String getTextFromValue(double value);

		/** This can be used to stop the mouse scroll-wheel from moving the slider.
		By default it's enabled.
		*/
		void setScrollWheelEnabled(bool enabled);

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
		virtual double proportionOfLengthToValue(double proportion);

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
		virtual double valueToProportionOfLength(double value);

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

		void restoreMouseIfHidden();

		void triggerChangeMessage(juce::NotificationType notification);

		//==============================================================================
		/** @internal */
		void paint(juce::Graphics& g) override;

		/** @internal */
		void resized() override;

		/** @internal */
		void mouseDown(const juce::MouseEvent& e) override;

		/** @internal */
		void mouseUp(const juce::MouseEvent&) override;

		/** @internal */
		void mouseDrag(const juce::MouseEvent& e) override;

		void handleAsyncUpdate() override;

		void valueChanged(juce::Value& value) override;

		void updateTextBoxEnablement();

		/** @internal */
		void mouseDoubleClick(const juce::MouseEvent&) override;

		double getMouseWheelData(double value, double wheelAmount);

		/** @internal */
		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

		/** @internal */
		void modifierKeysChanged(const juce::ModifierKeys&) override;

		/** @internal */
		void lookAndFeelChanged() override;

		/** @internal */
		void enablementChanged() override;

		/** @internal */
		void focusOfChildComponentChanged(FocusChangeType) override;

		/** @internal */
		void colourChanged() override;

		/** @internal */
		void mouseMove(const juce::MouseEvent& e) override;
		/** @internal */
		void mouseExit(const juce::MouseEvent&) override;
		/** @internal */
		void mouseEnter(const juce::MouseEvent& e) override;

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

		void textChanged();

		void updateText();

		void updateRange();

		double constrainedValue(double value) const;

		// based off LookAndFeel_V2
		void getSliderLayout();

		juce::Label* createRotarySliderTextBox();

		virtual void drawRotarySlider(juce::Graphics& g, float sliderPos);

		void sendDragStart();

		void sendDragEnd();

		//==============================================================================
		static double smallestAngleBetween(double a1, double a2) noexcept;

		virtual void handleRotaryDrag(const juce::MouseEvent& e);

		double limitAngleForRotaryDrag(const juce::MouseEvent& e, double angle, float minLegalAngle, float maxLegalAngle);
	private:

	};

	//================================================== DERIVED CLASS: TwoValuedRotarySlider ===================================//

	class TwoValuedRotarySlider : public RotarySlider {
	public:
		TwoValuedRotarySlider();
		~TwoValuedRotarySlider() override;

		// TODO
	};

	//================================================== DERIVED CLASS: PanRotarySlider =========================================//
#define PAN_MAX_MAGNITUDE 100

	class PanRotarySlider : public RotarySlider, public RotarySlider::Listener {
	public:
		PanRotarySlider();
		~PanRotarySlider() override;
		void sliderValueChanged(RotarySlider*) override;

		void setValue(double newValue, juce::NotificationType notification) override;

	protected:
		void drawRotarySlider(juce::Graphics& g, float sliderPos) override;
		void handleRotaryDrag(const juce::MouseEvent& e) override;
	private:
		// these are used if the slider range is being controlled by a second rotary slider
		bool hasExternalWidthController = false;
		juce::Value minValue, maxValue;
	};

};

#endif