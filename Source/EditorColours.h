#ifndef EDITORCOLOURS_H
#define EDITORCOLOURS_H

#pragma once
#include <JuceHeader.h>

namespace EditorColours
{
	const static juce::Colour darkblue{ juce::Colour::fromRGB(52, 52, 112) };
	const static juce::Colour lightblue{ juce::Colour::fromRGB(153, 153, 204) };
	const static juce::Colour black{ juce::Colour::fromRGB(11, 11, 12) };
	const static juce::Colour transparentBlack{ juce::Colour::fromRGBA(11, 11, 12, 30) };
	const static juce::Colour grey{ juce::Colour::fromRGB(48, 48, 51) };
	const static juce::Colour white{ juce::Colour::fromRGB(255, 255, 255) };
	const static juce::Colour meterRed{ juce::Colour::fromRGB(212,53,44) };
	const static juce::Colour meterDarkGreen{ juce::Colour::fromRGB(42,184,101) };
	const static juce::Colour meterLightGreen{ juce::Colour::fromRGB(96,187,71) };
}
#endif
