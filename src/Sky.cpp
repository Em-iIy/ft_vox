/*
Created by: Emily (Em_iIy) Winnink
Created on: 26/01/2026
*/

#include "Sky.hpp"
#include "SkyGradient.hpp"
#include "TerrainGenerator.hpp"

#include <array>

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

	_initNoise();
}

void	Sky::_initNoise()
{
	// Use for later optimisation
	return ;
	const int WIDTH = 256;
	const int HEIGHT = 256;
	const int DEPTH = 16;
	std::array<float, WIDTH * HEIGHT * DEPTH>	noise;

			// 0.3f, 0.6f,
			// 0.5f, 0.9f,
	NoiseSettings settings = {
		.spline = Spline({
			0.0f, 0.0f,
			1.0f, 1.0f
		}),
		.depth = 3,
		.step = 2.0f,
		.zoom = 16.0f
	};
	rng::fgen	gen = rng::generator(0.0f, 1.0f);
	for (int z = 0; z < DEPTH; ++z)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				int index = x + y * WIDTH + z * WIDTH * HEIGHT;
				noise[index] = rng::rand(gen);
				noise[index] = TerrainGenerator::noise3D(0, settings, mlm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
			}
		}
	}

	glGenTextures(1, &_noiseTex);
	glBindTexture(GL_TEXTURE_3D, _noiseTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, WIDTH, HEIGHT, DEPTH, 0, GL_RED, GL_FLOAT, &noise[0]);
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

float	Sky::getNightTimePercent() const
{
	float	nightTime = std::max(_time - _timeSettings.dayLen, 0.0f);
	return (nightTime / _timeSettings.nightLen);
}

float	Sky::_getTotalTime() const
{
	return (_timeSettings.dayLen + _timeSettings.nightLen);
}
