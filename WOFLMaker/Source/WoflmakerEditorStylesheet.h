#ifndef WOFLMAKEREDITORSTYLESHEET_H
#define WOFLMAKEREDITORSTYLESHEET_H

#include <JuceHeader.h>

/* a file to track macros for defining the look and feel of the plugin and its components */

//============================================= COLOURS ================================================//

/* Main App Window */
#define WOFL_APP_BACKGROUNDCOLOUR juce::Colours::black
#define WOFL_APP_BORDERCOLOUR juce::Colours::orange

/* Title */
#define WOFL_APP_TITLE_TEXTCOLOUR WOFL_APP_BORDERCOLOUR
#define WOFL_APP_TITLE_BORDERCOLOUR WOFL_APP_BORDERCOLOUR

/* Rotary Sliders */
#define WOFL_ROTARY_BODYCOLOUR juce::Colours::black.brighter()
#define WOFL_ROTARY_OUTLINECOLOUR juce::Colours::aquamarine
#define WOFL_ROTARY_THUMBCOLOUR juce::Colours::white
/* Width range indicators on pan slider */
#define WOFL_PAN_ROTARY_WIDTHINDICATORCOLOUR_LFO_DISABLED juce::Colours::darkgrey
#define WOFL_PAN_ROTARY_WIDTHINDICATORCOLOUR_LFO_ENABLED	juce::Colours::yellow

//============================================= DIMENSIONS ============================================//

/* App Starting Dimensions */
#define WOFL_APP_START_WIDTH 200
#define WOFL_APP_START_HEIGHT 600

/* App Title */
#define WOFL_APP_TITLE_FONTSIZE 15.0f

/* Dimensions with respect to parent bounds */
/* Proportional height macros should sum up to approx. 1.0 */
#define WOFL_APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.2f
// height of slider box w.r.t app height
#define WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.3f
/* other dimension macros */
#define WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.2f
#define WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.66f
#define WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.1f
#define WOFL_CONTROLBOX_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.5f
#define WOFL_CONTROLBOX_WIDTH_AS_PROPORTION_OF_APP_WIDTH 0.8f

#define WOFL_TOGGLEBUTTON_WIDTH_AS_PROPORTION_OF_CONTROLBOX_WIDTH 0.33f

//============================================= LAYOUT ===============================================//

/* Control box */
#define WOFL_CONTROLBOX_SLIDERBOX_ROWS 1
#define WOFL_CONTROLBOX_SLIDERBOX_COLS 2

#endif