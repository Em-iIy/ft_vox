/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#include "Sky.hpp"
#include "SkyGradient.hpp"

Sky::Sky()
{
}

Sky::Sky(const SkyDTO &dto)
{
	load(dto);
}


Sky::~Sky()
{
}

void	Sky::load(const SkyDTO &dto)
{
	_timeSettings = dto.timeSettings;

	_gradientStop0.load(dto.stop0, dto.timeSettings);
	_gradientStop1.load(dto.stop1, dto.timeSettings);
	_gradientStop2.load(dto.stop2, dto.timeSettings);
	_gradientStop3.load(dto.stop3, dto.timeSettings);
}
