#ifndef AUDIOPROCESSORVALUETREESTATEEXTENDED_H
#define AUDIOPROCESSORVALUETREESTATEEXTENDED_H

#include <JuceHeader.h>
#include "RotarySliders.h"

/* this is an (experimental) extension class for juce::AudioProcessorValueTreeState, to provide support for custom classes of mine */

// for listeners/attachments
class RotarySliderParameterAttachment : private RotarySlider::Listener {
public:
	RotarySliderParameterAttachment(juce::RangedAudioParameter& parameter, RotarySlider& slider, juce::UndoManager* undoManager = nullptr) :
		slider(slider), attachment(parameter, [this](float f) {setValue(f); }, undoManager)
	{
		slider.valueFromTextFunction = [&parameter](const juce::String& text) {return(double)parameter.convertFrom0to1(parameter.getValueForText(text)); };
		slider.textFromValueFunction = [&parameter](double value) {return parameter.getText(parameter.convertTo0to1((float)value), 0); };
		
		auto range = parameter.getNormalisableRange();
		auto convertFrom0to1Function = [range](double currentRangeStart, double currentRangeEnd, double normalisedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.convertFrom0to1((float)normalisedValue);
		};

		auto convertTo0To1Function = [range](double currentRangeStart, double currentRangeEnd, double mappedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.convertTo0to1((float)mappedValue);
		};

		auto snapToLegalValueFunction = [range](double currentRangeStart, double currentRangeEnd, double mappedValue) mutable {
			range.start = (float)currentRangeStart;
			range.end = (float)currentRangeEnd;
			return (double)range.snapToLegalValue((float)mappedValue);
		};

		juce::NormalisableRange<double> newRange{
			(double)range.start,
			(double)range.end,
			std::move(convertFrom0to1Function),
			std::move(convertTo0To1Function),
			std::move(snapToLegalValueFunction)
		};

		newRange.interval = range.interval;
		newRange.skew = range.skew;
		newRange.symmetricSkew = range.symmetricSkew;
		
		slider.setNormalisableRange(newRange);
		sendInitialUpdate();
		slider.valueChanged();
		slider.addListener(this);
	}

	~RotarySliderParameterAttachment() override { slider.removeListener(this); }

	void sendInitialUpdate() {
		attachment.sendInitialUpdate();
	}
private:
	void setValue(float newValue) {
		const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
		slider.setValue(newValue, juce::sendNotificationSync);
	}

	void sliderValueChanged(RotarySlider*) override {
		if (ignoreCallbacks || juce::ModifierKeys::currentModifiers.isRightButtonDown()) return;
		attachment.setValueAsPartOfGesture((float)slider.getValue());
	}

	void sliderDragStarted(RotarySlider*) override { attachment.beginGesture(); }
	void sliderDragEnded(RotarySlider*) override { attachment.endGesture(); }

	RotarySlider& slider;
	juce::ParameterAttachment attachment;
	bool ignoreCallbacks = false;
};

class AudioProcessorValueTreeStateExtended : public juce::AudioProcessorValueTreeState {
public:
	AudioProcessorValueTreeStateExtended(juce::AudioProcessor& processorToConnectTo, juce::UndoManager* undoManagerToUse) : juce::AudioProcessorValueTreeState(processorToConnectTo, undoManagerToUse) {}
	~AudioProcessorValueTreeStateExtended() override {}

	class JUCE_API RotarySliderAttachment {
	public:
		RotarySliderAttachment(AudioProcessorValueTreeStateExtended& stateToUse, const juce::String& parameterID, RotarySlider& slider) {
			if (auto* parameter = stateToUse.getParameter(parameterID))
				attachment = std::make_unique<RotarySliderParameterAttachment>(*parameter, slider, stateToUse.undoManager);
		}
	private:
		std::unique_ptr<RotarySliderParameterAttachment> attachment;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySliderAttachment)
	};
private:

};

#endif