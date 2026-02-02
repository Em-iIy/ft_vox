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

	_sun = dto.sun;
	_moon = dto.moon;

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

void	Sky::setGradient(Shader &shader)
{
	float timePercent = getTimePercent();
	
	shader.set_vec4("uStops", mlm::vec4(0.38f, 0.47f, 0.61f, 1.0f));
	shader.set_vec4("uColors[0]", _gradientStop0.sampleAt(timePercent));
	shader.set_vec4("uColors[1]", _gradientStop1.sampleAt(timePercent));
	shader.set_vec4("uColors[2]", _gradientStop2.sampleAt(timePercent));
	shader.set_vec4("uColors[3]", _gradientStop3.sampleAt(timePercent));
	shader.set_int("uStopCount", 4);
}

void	Sky::setSolarBodies(Shader &shader)
{
	shader.set_vec4("uSunDiskColor",_sun.diskColor);
	shader.set_float("uSunDiskFactor",_sun.diskFactor);
	shader.set_float("uSunDiskSize",_sun.diskSize);
	shader.set_vec4("uSunGlowColor",_sun.glowColor);
	shader.set_float("uSunGlowFactor",_sun.glowFactor);
	shader.set_float("uSunGlowSharpness",_sun.glowShaprness);

	shader.set_vec4("uMoonDiskColor",_moon.diskColor);
	shader.set_float("uMoonDiskFactor",_moon.diskFactor);
	shader.set_float("uMoonDiskSize",_moon.diskSize);
	shader.set_vec4("uMoonGlowColor",_moon.glowColor);
	shader.set_float("uMoonGlowFactor",_moon.glowFactor);
	shader.set_float("uMoonGlowSharpness",_moon.glowShaprness);
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
