/*
Created by: Emily (Em_iIy) Winnink
Created on: 20/10/2025
*/

#include "TerrainGenerator.hpp"
#include "Perlin.hpp"

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::TerrainGenerator(const TerrainGeneratorDTO &dto): _seed(dto.seed), _seaLevel(dto.seaLevel), _caveDiameter(dto.caveDiameter), _cave(dto.cave), _continentalness(dto.continentalness)
{
}

TerrainGenerator::~TerrainGenerator()
{
}

int	TerrainGenerator::getTerrainHeight(perlinSamplers &samplers, const mlm::ivec2 &pos)
{
	float	height = 0.0f;

	height += noise2D(samplers.height, _continentalness, static_cast<mlm::vec2>(pos));
	return (static_cast<int>(height));
}

int	TerrainGenerator::getTerrainHeight(perlinSamplers &samplers, const mlm::ivec3 &pos)
{
	return (getTerrainHeight(samplers, mlm::ivec2(pos.x, pos.z)));
}

Block	TerrainGenerator::getBlock(perlinSamplers &samplers, const mlm::ivec3 &pos, int terrainHeight)
{
	Block::Type	type = Block::STONE;
	bool		underwater = false;
	bool		sand = (terrainHeight > _seaLevel - 2 && terrainHeight <= _seaLevel + 2);

	if (pos.y > terrainHeight)
		type = Block::AIR;

	if (pos.y <= _seaLevel && terrainHeight < _seaLevel)
		underwater = true;
	

	// if (terrainHeight > _seaLevel - 2 && terrainHeight <= _seaLevel + 2)
	if (pos.y == terrainHeight)
	{

		if (sand)
			type = Block::SAND;
		else
			type = underwater ? Block::DIRT : Block::GRASS;
	}
	else if (pos.y < terrainHeight && pos.y > terrainHeight - 4)
	{
		if (sand)
			type = Block::SAND;
		else
			type = Block::DIRT;
	}

	// Only check for caves if block type is solid
	if (type != Block::AIR)
		if (isCave(samplers, pos))
			type = Block::AIR;

	if (type == Block::AIR && underwater)
		type = Block::WATER;

	Block ret(type);
	return (ret);
}

bool	TerrainGenerator::isCave(perlinSamplers &samplers, const mlm::ivec3 &pos)
{
	if (pos.y == 0)
		return (false);
	float	val1 = noise3D(samplers.cave1, _cave, pos);
	if (std::abs(val1) > _caveDiameter)
		return (false);
	float	val2 = noise3D(samplers.cave2, _cave, pos);
	if (std::abs(val2) > _caveDiameter)
		return (false);
	return (true);
}

perlinSamplers	TerrainGenerator::getSamplers()
{
	perlinSamplers	ret;
	ret.height.setSeed(_seed);
	ret.cave1.setSeed(_seed);
	ret.cave2.setSeed(_seed + 1);
	return (ret);
}

void	TerrainGenerator::setSeed(uint64_t seed)
{
	_seed = seed;
}

uint64_t	TerrainGenerator::getSeed() const
{
	return (_seed);
}

void	TerrainGenerator::setSeaLevel(int seaLevel)
{
	_seaLevel = seaLevel;
}

int	TerrainGenerator::getSeaLevel() const
{
	return (_seaLevel);
}

void	TerrainGenerator::setContinentalnessSpline(const Spline &spline)
{
	_continentalness.spline = spline;
}

const Spline	&TerrainGenerator::getContinentalnessSpline() const
{
	return (_continentalness.spline);
}

float	TerrainGenerator::noise2D(Perlin &sampler, const NoiseSettings &settings, const mlm::vec2 &pos)
{
	return (settings.spline.evaluate(_octaves2D(sampler, pos / settings.zoom, static_cast<uint64_t>(settings.depth), settings.step)));
}

float	TerrainGenerator::noise3D(Perlin &sampler, const NoiseSettings &settings, const mlm::vec3 &pos)
{
	return (settings.spline.evaluate(_octaves3D(sampler, pos / settings.zoom, static_cast<uint64_t>(settings.depth), settings.step)));
}

float	TerrainGenerator::_octaves2D(Perlin &sampler, const mlm::vec2 &pos, uint64_t depth, float step)
{
	float	ret = 0.0f;
	float	amplitude = 1.0f;
	float	frequency = 1.0f;

	for (; depth > 0; --depth)
	{
		float	temp = sampler.getValue(pos.x * frequency, pos.y * frequency) / amplitude;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		amplitude *= step;
		frequency *= step;
	}
	return (ret);
}

float	TerrainGenerator::_octaves3D(Perlin &sampler, const mlm::vec3 &pos, uint64_t depth, float step)
{
	float	ret = 0.0f;
	float	amplitude = 1.0f;
	float	frequency = 1.0f;

	for (; depth > 0; --depth)
	{
		float	temp = sampler.getValue(pos.x * frequency, pos.y * frequency, pos.z * frequency) / amplitude;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		amplitude *= step;
		frequency *= step;
	}
	return (ret);
}
