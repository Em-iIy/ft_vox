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

void	Sky::update(const float deltaTime)
{
	if (_paused == true)
		return ;
	_time += deltaTime;
	
	if (_time > _getTotalTime())
		_time -= _getTotalTime();
}

void	Sky::togglePause()
{
	_paused = !_paused;
}

float	Sky::getTime() const
{
	return (_time);
}

float	Sky::getTimePercent() const
{
	return (_time / _getTotalTime());
}

float	Sky::_getTotalTime() const
{
	return (_timeSettings.dayLen + _timeSettings.nightLen);
}
