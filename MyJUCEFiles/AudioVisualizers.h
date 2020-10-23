#pragma once

#include <JuceHeader.h>

#define NO_OVERLAY                    0
#define OVERLAY_INPUTS_AND_OUTPUTS    1
#define OVERLAY_CHANNELS              2
#define OVERLAY_ALL                   3

/* TODO: abandon AudioVisualiserComponent as a base class. Make a virtual base for ChannelInfo, and make a new virtual base class similar to AudioVisualiserComponent:
           - 4 channel colors, 1 background color; functions setChannelColour() and setBackgroundColour()
           - keep paint() same as AudioVisualiser. Change paintChannel()
           - ChannelInfo needs to be different for FFT functioning visualizers
*/

/* AUDIO VISUALISER */

class AudioVisualiser : public juce::AudioVisualiserComponent {
public:

    AudioVisualiser() : AudioVisualiserComponent(2) {
        numInputChannels = 2;
        numOutputChannels = 0;
        overlayChannels = NO_OVERLAY;
        backgroundColour = juce::Colours::black;
        setBufferSize(512);
        setSamplesPerBlock(256);
    }

    AudioVisualiser(int numInputChannels, int numOutputChannels, std::uint8_t overlayChannels) : 
            AudioVisualiserComponent(numInputChannels + numOutputChannels), overlayChannels(overlayChannels),
            numInputChannels(numInputChannels), numOutputChannels(numOutputChannels) 
    {
        setBufferSize(512);
        setSamplesPerBlock(256);
    }

    ~AudioVisualiser() override {}

    void setBackgroundColour(juce::Colour colour) {
        backgroundColour = colour;
    }

    void setChannelColour(unsigned int channel, juce::Colour colour) {
        jassert(channel < 4);
        // doesn't check to see if channel is out of range of current channels. Will keep this color saved in case the channel gets added later
        channelColours[channel] = colour;
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(backgroundColour);

        auto r = getLocalBounds().toFloat();

        switch (overlayChannels) {
            case NO_OVERLAY: /* every channel gets its own window */
                paintNoOverlay(g, r);
                break;
            case OVERLAY_INPUTS_AND_OUTPUTS: /* inputs are overlayed with each other in a single window , same for outputs */
                paintOverlayedIO(g, r);
                break;
            case OVERLAY_CHANNELS: /* each channel has its own window (ie, outputs and inputs of the same channel are overlayed with each other) */
                paintOverlayedChannels(g, r);
                break;
            case OVERLAY_ALL: /* everything overlayed in a single window */
                paintAllOverlayed(g, r);
                break;
            default: // NO_OVERLAY
                paintNoOverlay(g, r);
                break;
        }
    };

private:
    int numInputChannels, numOutputChannels;
    std::uint8_t overlayChannels = NO_OVERLAY;
    juce::Colour channelColours[4] = { juce::Colours::seagreen, juce::Colours::blue, juce::Colours::red, juce::Colours::purple };
    //std::vector<juce::Colour> componentColors;

    void paintNoOverlay(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelHeight = area.getHeight() / (float)(numInputChannels + numOutputChannels);
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(channelColours[channelColorIndex++]);//componentColors.at(channelColorIndex++));
            paintChannel(g, area.removeFromTop(channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
        }
    }

    // TODO: can potentially save computation time by NOT painting pixels that are already painted over?

    void paintOverlayedIO(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelHeight = (numInputChannels && numOutputChannels) ? 0.5f * area.getHeight() : area.getHeight();
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(channelColours[channelColorIndex]);//componentColors.at(channelColorIndex));
            if (channelColorIndex < numInputChannels - 1) { /* don't want to remove top from r yet, so instead I pass a new rectangle with proper dimensions */
                paintChannel(g, juce::Rectangle<float>(area.getX(), area.getY(), area.getWidth(), channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
            }
            else if (channelColorIndex == (numInputChannels - 1)) { /* final input (top) channel, so remove from r */
                paintChannel(g, area.removeFromTop(channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
            }
            else {
                paintChannel(g, area, c->levels.begin(), c->levels.size(), c->nextSample);
            }
            channelColorIndex++;
        }
    }

    void paintOverlayedChannels(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelHeight = (numInputChannels < 2 && numOutputChannels < 2) ? area.getHeight() : area.getHeight() * 0.5f;
        int numWindows = (numInputChannels > numOutputChannels) ? numInputChannels : numOutputChannels;
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(channelColours[channelColorIndex]);//componentColors.at(channelColorIndex));
            /* (channelColorIndex % numWindows) tells us which window to paint channel in */
            paintChannel(g, juce::Rectangle<float>(area.getX(), area.getY() + channelHeight * (channelColorIndex % numWindows), area.getWidth(), channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
            channelColorIndex++;
        }
    }

    void paintAllOverlayed(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(channelColours[channelColorIndex++]);//componentColors.at(channelColorIndex++));
            paintChannel(g, area, c->levels.begin(), c->levels.size(), c->nextSample);
        }
    }
};

/* FREQUENCY VISUALISER */

class FrequencyVisualiser : public AudioVisualiser {
public:
    FrequencyVisualiser() {}
    ~FrequencyVisualiser() override {}

    void paintChannel(juce::Graphics& g, juce::Rectangle<float> area, const juce::Range<float>* levels, int numLevels, int nextSample) override {
    
    }

private:
    bool ready = false;
};