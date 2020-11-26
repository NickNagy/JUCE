#ifndef COLOURSANDGRADIENTS_H
#define COLOURSANDGRADIENTS_H

#include <JuceHeader.h>

namespace magna {

	/*struct ColourOrGradient {
		union CG {
			juce::Colour colour;
			juce::ColourGradient gradient;
		};

		CG cg;
		bool isGradient;

		ColourOrGradient(juce::Colour c) : cg(c), isGradient(false) {}
		
		ColourOrGradient(juce::ColourGradient g) : cg(g), isGradient(true) {}

		~ColourOrGradient() {
			if (isGradient) {
				cg.colour.~Colour();
			}
			else {
				cg.gradient.~ColourGradient();
			}
		}
	};*/




};

#endif