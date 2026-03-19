/*
Created by: Emily (Em_iIy) Winnink
Created on: 13/10/2025
*/

#pragma once

#include <memory>

#include "Spline.hpp"
#include "Block.hpp"
#include "Perlin.hpp"

struct perlinSamplers {
	Perlin	height;
	Perlin	cave1;
	Perlin	cave2;
	Perlin	sand;
};

struct NoiseSettings {
	Spline	spline;
	float	depth = 1.0f;
	float	step = 1.0f;
	float	zoom = 100.0f;
};

struct TerrainGeneratorDTO {
	float			seed;
	float			seaLevel;
	float			caveDiameter;
	NoiseSettings	continentalness;
	NoiseSettings	cave;
	float			sandBeachThreshold;
	float			sandSeaThreshold;
	NoiseSettings	sand;
};

class TerrainGenerator {
	public:
		TerrainGenerator();
		TerrainGenerator(const TerrainGeneratorDTO &dto);
		~TerrainGenerator();

		int				getTerrainHeight(perlinSamplers &samplers, const mlm::ivec2 &pos);
		int				getTerrainHeight(perlinSamplers &samplers, const mlm::ivec3 &pos);


		Block			getBlock(perlinSamplers &samplers, const mlm::ivec3 &pos, int terrainHeight);
		bool			isCave(perlinSamplers &samplers, const mlm::ivec3 &pos);
		bool			isSand(perlinSamplers &samplers, const mlm::ivec2 &pos, int terrainHeight);

		perlinSamplers	getSamplers();

		void			setSeed(uint64_t seed);
		uint64_t		getSeed() const;

		void			setSeaLevel(int seaLevel);
		int				getSeaLevel() const;

		void			setContinentalnessSpline(const Spline &spline);
		const Spline	&getContinentalnessSpline() const;

		static float	noise2D(Perlin &sampler, const NoiseSettings &settings, const mlm::vec2 &pos);
		static float	noise3D(Perlin &sampler, const NoiseSettings &settings, const mlm::vec3 &pos);

	private:
		uint64_t		_seed;
		int				_seaLevel;
		float			_caveDiameter;
		NoiseSettings	_cave;
		float			_sandBeachThreshold;
		float			_sandSeaThreshold;
		NoiseSettings	_sand;
		NoiseSettings	_continentalness;

		static float	_octaves2D(Perlin &sampler, const mlm::vec2 &pos, uint64_t depth, float step);
		static float	_octaves3D(Perlin &sampler, const mlm::vec3 &pos, uint64_t depth, float step);
};

using TerrainGeneratorPtr = std::shared_ptr<TerrainGenerator>;
