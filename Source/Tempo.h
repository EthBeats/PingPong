/*
  ==============================================================================

    Tempo.h
    Created: 23 Jul 2024 4:20:44pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Tempo
{
public:
	Tempo();
	~Tempo();
	
	//==============================================================================
	void reset() noexcept;
	void update(const juce::AudioPlayHead* playhead) noexcept;
	double getMillisecondsForNoteLength(int index) const noexcept;
	double getTempo() const noexcept { return bpm; }
	
private:
	double bpm = 120.0;
	
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tempo)
};
