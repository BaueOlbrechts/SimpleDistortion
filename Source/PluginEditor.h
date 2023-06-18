/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "KnobStyle.h"
#include "LevelMeterStyle.h"
#include "DropDownStyle.h"
#include "DistortionGraphStyle.h"

//==============================================================================
/**
*/
class SimpleDistortionAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	SimpleDistortionAudioProcessorEditor(SimpleDistortionAudioProcessor&);
	~SimpleDistortionAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;


private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	SimpleDistortionAudioProcessor& audioProcessor;

	juce::Label titleLabel;

	//std::unique_ptr <KnobStyle> 
	KnobStyle driveKnob,
		hardnessKnob,
		mixKnob,
		outputGainKnob;
	juce::AudioProcessorValueTreeState::SliderAttachment driveKnobAttachment,
		hardnessKnobAttachment,
		mixKnobAttachment,
		outputGainKnobAttachment;


	LevelMeter lmInput, lmOutput;


	DropDownStyle clippingTypeDropDown;
	juce::AudioProcessorValueTreeState::ComboBoxAttachment clippingTypeDropDownAttachment;

	DistortionGraph distortionGraph;

	std::vector<juce::Component*> getComps();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDistortionAudioProcessorEditor)
};
