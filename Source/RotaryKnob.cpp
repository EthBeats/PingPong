/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 1 Jul 2024 2:13:35pm
    Author:  Ethan Miller

  ==============================================================================
*/

#include "RotaryKnob.h"

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String& text,
					   juce::AudioProcessorValueTreeState& apvts,
					   const juce::ParameterID& parameterID,
					   bool drawFromMiddle)
	: attachment(apvts, parameterID.getParamID(), slider)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    // slider component setup
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
	slider.setBounds(0, 0, 70, 86);
	addAndMakeVisible(slider);
	
	// label component setup
	label.setText(text, juce::NotificationType::dontSendNotification);
	label.setJustificationType(juce::Justification::horizontallyCentred);
	label.setBorderSize(juce::BorderSize<int> { 0, 0, 2, 0 });
	label.attachToComponent(&slider, false);
	addAndMakeVisible(label);
	
	setSize(70, 110);
	
	// set up slider look and feel
    setLookAndFeel(RotaryKnobLookAndFeel::get());
    
    slider.setRotaryParameters(1.25f * juce::MathConstants<float>::pi,
							   2.75f * juce::MathConstants<float>::pi, true);
	slider.getProperties().set("drawFromMiddle", drawFromMiddle);
}

RotaryKnob::~RotaryKnob()
{
}

//==============================================================================
void RotaryKnob::resized()
{
	// set slider position
	slider.setTopLeftPosition(0, 24);

	// debug statement for making sure all sliders are visible
    DBG("Slider is visible: " << (slider.isVisible() ? "true" : "false") << '\n');
}
