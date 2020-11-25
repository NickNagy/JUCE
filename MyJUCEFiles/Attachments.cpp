#include "Attachments.h"

namespace magna {
	RotarySliderParameterAttachment::RotarySliderParameterAttachment(juce::RangedAudioParameter& parameter, RotarySlider& slider, juce::UndoManager* undoManager) :
		slider(slider), attachment(parameter, [this](float f) { setValue(f); }, undoManager) {

		slider.valueFromTextFunction = [&parameter](const juce::String& text) {return(double)parameter.convertFrom0to1(parameter.getValueForText(text)); };
		slider.textFromValueFunction = [&parameter](double value) { return parameter.getText(parameter.convertTo0to1((float)value), 0); };

		// NOTE: skipped over for now: double-click handle

		auto range = parameter.getNormalisableRange();

		auto convertFrom0To1Function = [range](double currentRangeStart, double currentRangeEnd, double normalisedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.convertFrom0to1((float)normalisedValue);
		};

		auto convertTo0to1Function = [range](double currentRangeStart, double currentRangeEnd, double mappedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.convertTo0to1((float)mappedValue);
		};

		auto snapToLegalValueFunction = [range](double currentRangeStart, double currentRangeEnd, double mappedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.snapToLegalValue((float)mappedValue);
		};

		juce::NormalisableRange<double> newRange{ (double)range.start, (double)range.end, std::move(convertFrom0To1Function), std::move(convertTo0to1Function), std::move(snapToLegalValueFunction) };
		newRange.interval = range.interval;
		newRange.skew = range.skew; // NOTE: I have not implemented skew handling for RotarySlider yet
		newRange.symmetricSkew = range.symmetricSkew;

		slider.setNormalisableRange(newRange);
		sendInitialUpdate();
		slider.valueChanged();
		slider.addListener(this);
	}

	RotarySliderParameterAttachment::~RotarySliderParameterAttachment() {
		slider.removeListener(this);
	}

	void RotarySliderParameterAttachment::sendInitialUpdate() { attachment.sendInitialUpdate(); }

	void RotarySliderParameterAttachment::setValue(float newValue) {
		const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
		slider.setValue(newValue, juce::sendNotificationSync);
	}

	void RotarySliderParameterAttachment::sliderValueChanged(RotarySlider*) {
		// Not sure I need the isRightButtonDown() check for my purposes
		if (ignoreCallbacks || juce::ModifierKeys::currentModifiers.isRightButtonDown()) {
			return;
		}
		attachment.setValueAsPartOfGesture((float)slider.getValue());
	}
};