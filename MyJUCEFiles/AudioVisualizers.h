#pragma once

#include <JuceHeader.h>

#define NO_OVERLAY                    0
#define OVERLAY_INPUTS_AND_OUTPUTS    1
#define OVERLAY_CHANNELS              2
#define OVERLAY_ALL                   3

class AudioVisualizer : public juce::AudioVisualiserComponent {
public:

    AudioVisualizer() : AudioVisualiserComponent(2) {
        numInputChannels = 2;
        numOutputChannels = 0;
        overlayChannels = NO_OVERLAY;
        componentColors = { juce::Colours::black, juce::Colours::seagreen, juce::Colours::cyan };
        setBufferSize(512);
        setSamplesPerBlock(256);
    }

    AudioVisualizer(int numInputChannels, int numOutputChannels, std::uint8_t overlayChannels, std::vector<juce::Colour>& componentColors) : 
            AudioVisualiserComponent(numInputChannels + numOutputChannels), componentColors(componentColors), overlayChannels(overlayChannels),
            numInputChannels(numInputChannels), numOutputChannels(numOutputChannels) 
    {
        setBufferSize(512);
        setSamplesPerBlock(256);
    }

    ~AudioVisualizer() override {}

    void setColours(std::vector<juce::Colour>& componentColors) {
        this->componentColors = componentColors;
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(componentColors.at(0));

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
    std::vector<juce::Colour> componentColors;

    void paintNoOverlay(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelHeight = area.getHeight() / (float)(numInputChannels + numOutputChannels);
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(componentColors.at(channelColorIndex++));
            paintChannel(g, area.removeFromTop(channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
        }
    }

    void paintOverlayedIO(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelHeight = (numInputChannels && numOutputChannels) ? 0.5f * area.getHeight() : area.getHeight();
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(componentColors.at(channelColorIndex));
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
            g.setColour(componentColors.at(channelColorIndex));
            /* (channelColorIndex % numWindows) tells us which window to paint channel in */
            paintChannel(g, juce::Rectangle<float>(area.getX(), area.getY() + channelHeight * (channelColorIndex % numWindows), area.getWidth(), channelHeight), c->levels.begin(), c->levels.size(), c->nextSample);
            channelColorIndex++;
        }
    }

    void paintAllOverlayed(juce::Graphics& g, juce::Rectangle<float>& area) {
        auto channelColorIndex = 1;
        for (auto* c : channels) {
            g.setColour(componentColors.at(channelColorIndex++));
            paintChannel(g, area, c->levels.begin(), c->levels.size(), c->nextSample);
        }
    }
};
