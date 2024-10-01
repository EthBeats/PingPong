/*
  ==============================================================================

    LookAndFeel.h
    Created: 6 Jul 2024 11:30:20am
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
//namespace Colors
//{
//	const juce::Colour backgroundTop { 245, 240, 235 };
//	const juce::Colour backgroundBottom { 255, 255, 255 };
//	const juce::Colour header { 40, 40, 40 };
//	
//	namespace Knob
//	{
//		const juce::Colour trackBackground { 205, 200, 195 };
//		const juce::Colour trackActive { 177, 101, 135 };
//		const juce::Colour outline { 255, 250, 245 };
//		const juce::Colour gradientTop { 250, 245, 240 };
//		const juce::Colour gradientBottom { 240, 235, 230 };
//		const juce::Colour dial { 100, 100, 100 };
//		const juce::Colour dropShadow { 195, 190, 185 };
//		const juce::Colour label { 80, 80, 80 };
//		const juce::Colour textBoxBackground { 80, 80, 80 };
//		const juce::Colour value { 240, 240, 240 };
//		const juce::Colour caret { 255, 255, 255 };
//	}
//	
//	namespace Group
//	{
//		const juce::Colour label { 160, 155, 150 };
//		const juce::Colour outline { 235, 230, 225 };
//	}
//	
//	namespace Button
//	{
//		const juce::Colour text { 80, 80, 80 };
//		const juce::Colour textToggled { 40, 40, 40 };
//		const juce::Colour background { 245, 240, 235 };
//		const juce::Colour backgroundToggled { 255, 250, 245 };
//		const juce::Colour outline { 235, 230, 225 };
//	}
//	
//	namespace LevelMeter
//	{
//		const juce::Colour background { 245, 240, 235 };
//		const juce::Colour tickLine { 200, 200, 200 };
//		const juce::Colour tickLabel { 80, 80, 80 };
//		const juce::Colour tooLoud { 226, 74, 81 };
//		const juce::Colour levelOK { 65, 206, 88 };
//	}
//}

//==============================================================================
namespace Colors
{
	const juce::Colour backgroundTop { 40, 40, 40 };
	const juce::Colour backgroundBottom { 55, 70, 85 };
	const juce::Colour header { 40, 40, 40 };
	
	namespace Knob
	{
		const juce::Colour trackBackground { 85, 85, 85 };
		const juce::Colour trackActive { 200, 100, 0 };
		const juce::Colour outline { 37, 37, 37 };
		const juce::Colour gradientTop { 57, 57, 57 };
		const juce::Colour gradientBottom { 37, 37, 37 };
		const juce::Colour dial { 150, 150, 150 };
		const juce::Colour dropShadow { 15, 15, 15 };
		const juce::Colour label { 150, 150, 150 };
		const juce::Colour textBoxBackground { 150, 150, 150 };
		const juce::Colour value { 240, 240, 240 };
		const juce::Colour caret { 255, 255, 255 };
	}
	
	namespace Group
	{
		const juce::Colour label { 150, 150, 150 };
		const juce::Colour outlineTop { 150, 150, 150 };
		const juce::Colour outlineBottom { 57, 57, 57 };
	}
	
	namespace Button
	{
		const juce::Colour text { 85, 85, 85 };
		const juce::Colour textToggled { 200, 100, 0 };
		const juce::Colour background { 37, 37, 37 };
		const juce::Colour backgroundToggled { 37, 37, 37 };
		const juce::Colour outline { 85, 85, 85 };
	}
	
	namespace LevelMeter
	{
		const juce::Colour tickLine { 150, 150, 150 };
		const juce::Colour tickLabel { 150, 150, 150 };
		const juce::Colour tooLoud { 226, 74, 81 };
		const juce::Colour levelOK { 65, 206, 88 };
	}
}

//==============================================================================
/**
*/
class Fonts
{
public:
	Fonts() = delete;
	
	//==============================================================================
	static juce::Font getFont(float height = 16.f);

private:
	static const juce::Typeface::Ptr typeface;
};

//==============================================================================
/**
*/
class RotaryKnobLookAndFeel  : public juce::LookAndFeel_V4
{
public:
	RotaryKnobLookAndFeel();
	~RotaryKnobLookAndFeel() override;

	//==============================================================================
	static RotaryKnobLookAndFeel* get();
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
						  float sliderPos, const float rotaryStartAngle,
						  const float rotaryEndAngle, juce::Slider& slider) override;
	
	juce::Font getLabelFont(juce::Label& label) override;
	juce::Label* createSliderTextBox(juce::Slider& slider) override;
	void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override { }
	void fillTextEditorBackground(juce::Graphics& g, int width, int height,
								  juce::TextEditor& textEditor) override;

private:
	juce::DropShadow dropShadow { Colors::Knob::dropShadow, 6,
								  juce::Point<int> { 0, 3 } };
	
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnobLookAndFeel)
};

//==============================================================================
/**
*/
class MainLookAndFeel  : public juce::LookAndFeel_V4
{
public:
	MainLookAndFeel();
	~MainLookAndFeel() override;
	
	//==============================================================================
	void drawGroupComponentOutline (juce::Graphics& g, int width, int height,
									const juce::String& text,
									const juce::Justification& position,
									juce::GroupComponent& group) override;
	juce::Font getLabelFont(juce::Label& label) override;
	
private:

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainLookAndFeel)
};

//==============================================================================
/**
*/
class ButtonLookAndFeel  : public juce::LookAndFeel_V4
{
public:
	ButtonLookAndFeel();
	~ButtonLookAndFeel() override;
	
	//==============================================================================
	static ButtonLookAndFeel* get();
	void drawButtonBackground(juce::Graphics& g, juce::Button& button,
							  const juce::Colour& backgroundColour,
							  bool shouldDrawButtonAsHighlighted,
							  bool shouldDrawButtonAsDown) override;
	void drawButtonText(juce::Graphics& g, juce::TextButton& button,
						bool shouldDrawButtonAsHighlighted,
						bool shouldDrawButtonAsDown) override;
	
private:

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonLookAndFeel)
};
