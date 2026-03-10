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

int	TerrainGenerator::getTerrainHeight(const mlm::ivec2 &pos)
{
	float	height = 0.0f;

	height += noise2D(_seed, _continentalness, static_cast<mlm::vec2>(pos));
	return (static_cast<int>(height));
}

int	TerrainGenerator::getTerrainHeight(const mlm::ivec3 &pos)
{
	return (getTerrainHeight(mlm::ivec2(pos.x, pos.z)));
}

Block	TerrainGenerator::getBlock(const mlm::ivec3 &pos, int terrainHeight)
{
	Block::Type	type = Block::STONE;
	bool		underwater = false;

	if (pos.y > terrainHeight)
		type = Block::AIR;

	if (pos.y <= _seaLevel && terrainHeight < _seaLevel)
		underwater = true;
	
	if (pos.y == terrainHeight)
		type = underwater ? Block::DIRT : Block::GRASS;
	else if (pos.y < terrainHeight && pos.y > terrainHeight - 4)
		type = Block::DIRT;

	// Only check for caves if block type is solid
	if (type != Block::AIR)
		if (isCave(pos))
			type = Block::AIR;

	if (type == Block::AIR && underwater)
		type = Block::WATER;

	Block ret(type);
	return (ret);
}

bool	TerrainGenerator::isCave(const mlm::ivec3 &pos)
{
	if (pos.y == 0)
		return (false);
	float	val1 = noise3D(_seed, _cave, pos);
	if (std::abs(val1) > _caveDiameter)
		return (false);
	float	val2 = noise3D(_seed + 1, _cave, pos);
	if (std::abs(val2) > _caveDiameter)
		return (false);
	return (true);
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

float	TerrainGenerator::noise2D(uint64_t seed, const NoiseSettings &settings, const mlm::vec2 &pos)
{
	return (settings.spline.evaluate(_octaves2D(seed, pos / settings.zoom, static_cast<uint64_t>(settings.depth), settings.step)));
}

float	TerrainGenerator::noise3D(uint64_t seed, const NoiseSettings &settings, const mlm::vec3 &pos)
{
	return (settings.spline.evaluate(_octaves3D(seed, pos / settings.zoom, static_cast<uint64_t>(settings.depth), settings.step)));
}

float	TerrainGenerator::_octaves2D(uint64_t seed, const mlm::vec2 &pos, uint64_t depth, float step)
{
	float	ret = 0.0f;
	float	amplitude = 1.0f;
	float	frequency = 1.0f;
	Perlin	noise;

	noise.setSeed(seed);
	for (; depth > 0; --depth)
	{
		float	temp = noise.getValue(pos.x * frequency, pos.y * frequency) / amplitude;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		amplitude *= step;
		frequency *= step;
	}
	return (ret);
}

float	TerrainGenerator::_octaves3D(uint64_t seed, const mlm::vec3 &pos, uint64_t depth, float step)
{
	float	ret = 0.0f;
	float	amplitude = 1.0f;
	float	frequency = 1.0f;
	Perlin	noise;

	noise.setSeed(seed);
	for (; depth > 0; --depth)
	{
		float	temp = noise.getValue(pos.x * frequency, pos.y * frequency, pos.z * frequency) / amplitude;
		if (std::abs(temp) > std::numeric_limits<float>::epsilon())
			ret += temp;
		else
			break ;
		amplitude *= step;
		frequency *= step;
	}
	return (ret);
}
