#ifndef PARAMETERS_H
#define PARAMETERS_H

#pragma once
#include <JuceHeader.h>

class Parameters
{
public:
    inline static const juce::String
        ID_INPUT = "input",
        ID_INPUT_DISPLAY = "Input Gain",
        ID_OUTPUT = "output",
        ID_OUTPUT_DISPLAY = "Output Gain",
        ID_MIX = "mix",
        ID_MIX_DISPLAY = "Mix",
        ID_DRIVE = "drive",
        ID_DRIVE_DISPLAY = "Drive/Hardness",
        ID_CLIPPINGTYPE = "clippingType",
        ID_CLIPPINGTYPE_DISPLAY = "Clipping Type",
        ID_BYPASS = "bypass",
        ID_BYPASS_DISPLAY = "Bypass";

    enum class ClippingType
    {
        SoftClipping,
        HardClipping,
        Dummy1,
        Dummy2
    };

    static const char* ClippingTypeToString(ClippingType ct) noexcept
    {
        switch(ct)
        {
            case ClippingType::SoftClipping:
                return "Soft Clipping";
            case ClippingType::HardClipping:
                return "Hard Clipping";
            case ClippingType::Dummy1:
                return "Dummy 1";
            case ClippingType::Dummy2:
                return "Dummy 2";
        }
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            ID_DRIVE,
            ID_DRIVE_DISPLAY,
            juce::NormalisableRange<float>(0.0f, 1.0f),
            0.5f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            ID_INPUT,
            ID_INPUT_DISPLAY,
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            ID_OUTPUT,
            ID_OUTPUT_DISPLAY,
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            ID_MIX,
            ID_MIX_DISPLAY,
            juce::NormalisableRange<float>(0.0f, 1.0f),
            0.5f));


        juce::StringArray clippingTypeArray(ClippingTypeToString(ClippingType::HardClipping),
            ClippingTypeToString(ClippingType::SoftClipping),
            ClippingTypeToString(ClippingType::Dummy1),
            ClippingTypeToString(ClippingType::Dummy2));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            ID_CLIPPINGTYPE,
            ID_CLIPPINGTYPE_DISPLAY,
            clippingTypeArray,
            0));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            ID_BYPASS,
            ID_BYPASS_DISPLAY,
            false));


        return layout;
    }
};

#endif
