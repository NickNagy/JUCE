#ifndef COMPONENT_H
#define COMPONENT_H

#include <JuceHeader.h>

/* JUCE components like juce::Slider use enums for colorIds, but these are difficult to extend when creating derived classes. Instead, children of base class magna::Component can
use a colourMap for defining and fetching colours*/
namespace magna {
	class Component {
	public:
		void setColour(std::string id, juce::Colour colour) {
			colourMap[id] = colour;
		}

		juce::Colour getColour(std::string id) {
			return colourMap[id];
		}
	protected:
		std::map<std::string, juce::Colour> colourMap;
	};
}

#endif