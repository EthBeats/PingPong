/*
  ==============================================================================

    Measurement.h
    Created: 29 Jul 2024 6:19:36pm
    Author:  Ethan Miller

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
struct Measurement
{
	void reset() noexcept
	{
		value.store(0.f);
	}
	
	void updateIfGreater(float newValue) noexcept
	{
		auto oldValue = value.load();
		
		while (newValue > oldValue && !value.compare_exchange_weak(oldValue, newValue));
	}
	
	float readAndReset() noexcept
	{
		return value.exchange(0.0f);
	}
	
	std::atomic<float> value;
};
