/*
  ==============================================================================

    Tempo.cpp
    Created: 23 Jul 2024 4:20:44pm
    Author:  Ethan Miller

  ==============================================================================
*/

#include "Tempo.h"

static std::array<double, 16> noteLengthMultipliers = {
	0.125,
	0.5 / 3.0,
	0.1875,
	0.25,
	1.0 / 3.0,
	0.375,
	0.5,
	2.0 / 3.0,
	0.75,
	1.0,
	4.0 / 3.0,
	1.5,
	2.0,
	8.0 / 3.0,
	3.0,
	4.0,
};

//==============================================================================
Tempo::Tempo()
{
}

Tempo::~Tempo()
{
}

//==============================================================================
void Tempo::reset() noexcept
{
	bpm = 120.0;
}

void Tempo::update(const juce::AudioPlayHead* playhead) noexcept
{
	reset();
	
	if (playhead == nullptr) { return; }
	
	const auto opt = playhead->getPosition();
	
	if (!opt.hasValue()) { return; }
	
	const auto& pos = *opt;
	
	if (pos.getBpm().hasValue()) {
		bpm = *pos.getBpm();
	}
}

double Tempo::getMillisecondsForNoteLength(int index) const noexcept
{
	return 60000.0 * noteLengthMultipliers[size_t(index)] / bpm;
}
