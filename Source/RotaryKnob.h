/*
  ==============================================================================

    RotaryKnob.h
    Created: 1 Jul 2024 2:13:35pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class RotaryKnob  : public juce::Component
{
public:
    RotaryKnob(const juce::String& text,
			   juce::AudioProcessorValueTreeState& apvts,
			   const juce::ParameterID& parameterID,
			   bool drawFromMiddle = false);
    ~RotaryKnob() override;
    
    //==============================================================================
    void resized() override;
    
    juce::Slider slider;
    juce::Label label;
    
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

private:

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};
