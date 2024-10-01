/*
  ==============================================================================

    LevelMeter.h
    Created: 24 Jul 2024 4:47:34pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "Measurement.h"

//==============================================================================
/**
*/
class LevelMeter  : public juce::Component, private juce::Timer
{
public:
	LevelMeter(Measurement& measurementL,
			   Measurement& measurementR);
	~LevelMeter() override;
	
	//==============================================================================
	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	void timerCallback() override;
	
	int positionForLevel(float dbLevel) const noexcept
	{
		return int(std::round(juce::jmap(dbLevel, maxdB, mindB, maxPos, minPos)));
	}
	
	void drawLevel(juce::Graphics& g, float level, int x, int width);
	void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;
	
	Measurement& measurementL;
	Measurement& measurementR;
	
	static constexpr float maxdB = 6.f;
	static constexpr float mindB = -60.f;
	static constexpr float stepdB = 6.f;
	static constexpr float clampdB = -120.f;
	static constexpr float clampLevel = 0.000001f;
	static constexpr int refreshRate = 60;
	
	float maxPos = 0.f;
	float minPos = 0.f;
	float dbLevelL;
	float dbLevelR;
	float decay = 0.f;
	float levelL = clampLevel;
	float levelR = clampLevel;
	
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
