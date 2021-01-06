#ifndef WOFLMAKEREDITORSTYLESHEET_H
#define WOFLMAKEREDITORSTYLESHEET_H

#include <JuceHeader.h>

/* a file to track macros for defining the look and feel of the plugin and its components */

//============================================= COLOURS ================================================//

/* Main App Window */
#define WOFL_APP_BACKGROUNDCOLOUR juce::Colours::silver.brighter()
#define WOFL_APP_BORDERCOLOUR juce::Colours::orange

/* Title */
#define WOFL_APP_TITLE_TEXTCOLOUR WOFL_APP_BORDERCOLOUR
#define WOFL_APP_TITLE_BORDERCOLOUR WOFL_APP_BORDERCOLOUR

/* Control Box */
#define WOFL_CONTROLBOX_BACKGROUNDCOLOUR juce::Colours::darkgrey
#define WOFL_CONTROLBOX_BORDERCOLOUR juce::Colours::black

/* Rotary Sliders */
#define WOFL_ROTARY_BODYCOLOUR juce::Colours::black.brighter()
#define WOFL_ROTARY_OUTLINECOLOUR juce::Colours::aquamarine
#define WOFL_ROTARY_THUMBCOLOUR juce::Colours::white
/* Width range indicators on pan slider */
#define WOFL_PAN_ROTARY_WIDTHINDICATORCOLOUR_LFO_DISABLED juce::Colours::darkgrey
#define WOFL_PAN_ROTARY_WIDTHINDICATORCOLOUR_LFO_ENABLED	juce::Colours::yellow

//============================================= IMAGES ================================================//

#define GET_IMAGE(_image_, _size_) juce::ImageCache::getFromMemory(_image_, _size_)

/* Image files */
#define GET_CAST_IRON_IMAGE GET_IMAGE(BinaryData::_3806905090_ce4e1f6c7e_o_jpg, BinaryData::_3806905090_ce4e1f6c7e_o_jpgSize)
#define GET_WAFFLE_IMAGE GET_IMAGE(BinaryData::wp2418964_jpg, BinaryData::wp2418964_jpgSize)

/* Fill-type */
#define WOFL_IMAGE_RECTANGLE_FILLTYPE juce::RectanglePlacement::Flags::stretchToFit

/* App background image */
#define WOFL_GET_APP_BACKGROUND_IMAGE GET_CAST_IRON_IMAGE

/* Pan center sliderbox image */
#define WOFL_GET_PAN_ROTARY_SLIDERBOX_IMAGE GET_CAST_IRON_IMAGE

/* App title background image */
#define WOFL_GET_APP_TITLE_BACKGROUND_IMAGE GET_CAST_IRON_IMAGE

/* Controlbox background image */
#define WOFL_GET_CONTROLBOX_BACKGROUND_IMAGE GET_CAST_IRON_IMAGE

#define USING_IMAGES 0

//============================================= FONTS =================================================//

#define GET_FONT(_font_, _size_) juce::Typeface::createSystemTypefaceFor(_font_, _size_)

/* App title font */
#define WOFL_GET_APP_TITLE_FONT GET_FONT(BinaryData::BAUHS93_TTF, BinaryData::BAUHS93_TTFSize)

/* Component title font */
#define WOFL_GET_COMPONENT_TITLE_FONT GET_FONT(BinaryData::BROADW_TTF, BinaryData::BAUHS93_TTFSize)

/* Digital font */
#define WOFL_GET_DIGITAL_FONT GET_FONT(BinaryData::OCRAEXT_TTF, BinaryData::OCRAEXT_TTFSize)

//============================================= DIMENSIONS ============================================//

/* App Starting Dimensions */
#define WOFL_APP_START_WIDTH 200
#define WOFL_APP_START_HEIGHT 600

/* App Title */
#define WOFL_APP_TITLE_FONTSIZE 15.0f

/* Dimensions with respect to control box */
#define WOFL_LFO_WINDOW_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.2f
#define WOFL_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.66f
#define WOFL_TOGGLEBUTTON_HEIGHT_AS_PROPORTION_OF_CONTROLBOX_HEIGHT 0.1f
#define WOFL_TOGGLEBUTTON_WIDTH_AS_PROPORTION_OF_CONTROLBOX_WIDTH 0.33f
#define WOFL_CONTROLBOX_SPACE_LFO_WINDOW_REDUCTION_FACTOR 0.1f

/* Dimensions with respect to main app */
#define WOFL_APP_TITLE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.15f
#define WOFL_PAN_SLIDERBOX_SPACE_HEIGHT_AS_PROPORTION_OF_APP_HEIGHT 0.2f
#define WOFL_APP_SPACE_CONTROLBOX_REDUCTION_FACTOR 0.1f

//============================================= LAYOUT ===============================================//

/* Control box */
#define WOFL_CONTROLBOX_SLIDERBOX_ROWS 1
#define WOFL_CONTROLBOX_SLIDERBOX_COLS 2

#endif