/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#include "SkyGradient.hpp"
#include "Sky.hpp"

#include <algorithm>

SkyGradient::SkyGradient()
{
}

SkyGradient::SkyGradient(const GradientDTO &dto, const SkyTimeSettings &timeSettings)
{
	load(dto, timeSettings);
}

SkyGradient::~SkyGradient()
{
}

void	SkyGradient::load(const GradientDTO &dto, const SkyTimeSettings &timeSettings)
{
	const float	sunrisePartOfDay = timeSettings.sunriseLen / timeSettings.dayLen;
	const float	sunsetPartOfDay = timeSettings.sunsetLen / timeSettings.dayLen;
	const float	sunrisePartOfNight = timeSettings.sunriseLen / timeSettings.nightLen;
	const float	sunsetPartOfNight = timeSettings.sunsetLen / timeSettings.nightLen;

	const float	sunriseEnd = sunrisePartOfDay * 0.5f;
	const float	sunsetStart = 0.5f - sunsetPartOfDay * 0.5f;
	const float	sunsetEnd = 0.5f + sunsetPartOfNight * 0.5f;
	const float	sunriseStart = 0.5f - sunrisePartOfNight * 0.5f;

	_stops.clear();
	_stops.reserve(9);
	_stops.emplace_back(0.0f, dto.sunriseCol);
	_stops.emplace_back(sunriseEnd, dto.dayLowCol);
	_stops.emplace_back((sunriseEnd + sunsetStart) * 0.5f, dto.dayHighCol);
	_stops.emplace_back(sunsetStart, dto.dayLowCol);
	_stops.emplace_back(0.5f, dto.sunsetCol);
	_stops.emplace_back(sunsetEnd, dto.nightLowCol);
	_stops.emplace_back((sunsetEnd + sunriseStart) * 0.5f, dto.nightHighCol);
	_stops.emplace_back(sunriseStart, dto.nightLowCol);
	_stops.emplace_back(1.0f, dto.sunriseCol);

	std::sort(_stops.begin(), _stops.end(), 
		[](const auto &a, const auto &b)
		{
			return (a.first < b.first);
		}
	);
}

mlm::vec4	SkyGradient::sampleAt(const float t) const
{
	auto stop = std::find_if(_stops.begin(), _stops.end(), 
		[t](const auto &a)
		{
			return (a.first < t);
		}
	);
	if (stop == _stops.begin())
		return (_stops.front().second);
	if (stop == _stops.end())
		return (_stops.back().second);
	// basic return (no blending)
	return (stop->second);
}

