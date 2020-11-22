#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H

#include <JuceHeader.h>
#include "RotarySliders.h"

class RotarySliderParameterAttachment : private RotarySlider::Listener {
public:
	RotarySliderParameterAttachment(juce::RangedAudioParameter& parameter, RotarySlider& slider, juce::UndoManager* undoManager = nullptr);
	~RotarySliderParameterAttachment() override;
	void sendInitialUpdate();
private:
	RotarySlider& slider;
	juce::ParameterAttachment attachment;
	bool ignoreCallbacks = false;

	void setValue(float newValue);
	void sliderValueChanged(RotarySlider*) override;
	void sliderDragStarted(RotarySlider*) override { attachment.beginGesture(); }
	void sliderDragEnded(RotarySlider*) override { attachment.endGesture(); }
};

#endif