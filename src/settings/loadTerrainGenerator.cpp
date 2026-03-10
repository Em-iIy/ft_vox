/*
Created by: Emily (Em_iIy) Winnink
Created on: 22/10/2025
*/

#include "Settings.hpp"

static void	loadSpline(Spline &target, JSON::NodePtr node)
{
	JSON::ListPtr			array = node->getList();
	std::size_t				len = array->size();
	std::vector<mlm::vec2>	points;

	if (len == 0 || len % 2 != 0)
		throw std::runtime_error("terrainGenerator spline can't be empty and must contain an even amount of points");
	points.reserve(len / 2);
	for (std::size_t i = 0; i + 1 < len; i += 2)
	{
		mlm::vec2 temp = mlm::vec2((*array)[i]->getNumber(), (*array)[i + 1]->getNumber());
		points.push_back(temp);
	}
	target.setPoints(points);
}

static void	loadNoiseSettings(NoiseSettings &target, JSON::NodePtr node)
{
	loadSpline(target.spline, node->get("spline"));
	target.depth = node->get("depth")->getNumber();
	target.step = node->get("step")->getNumber();
	target.zoom = node->get("zoom")->getNumber();
	if (target.depth < 1.0f)
		throw std::runtime_error("terrainGenerator: noiseSettings: depth can't be negative");
	if (target.depth > static_cast<float>(std::numeric_limits<uint64_t>::max()))
		throw std::runtime_error("terrainGenerator: noiseSettings: depth can't be larger than " + std::to_string(std::numeric_limits<uint64_t>::max()));
	if (target.step < 1.0f)
		throw std::runtime_error("terrainGenerator: noiseSettings: step can't be smaller than 1");
	if (target.zoom < 1.0f)
		throw std::runtime_error("terrainGenerator: noiseSettings: zoom can't be smaller than 1");
}

static void	validateSettings(const TerrainGeneratorDTO &terrainDto)
{
	if (terrainDto.seed < 0.0f)
		throw std::runtime_error("terrainGenerator: seed can't be negative");
	if (terrainDto.seed > static_cast<float>(std::numeric_limits<uint64_t>::max()))
		throw std::runtime_error("terrainGenerator: seed can't be larger than " + std::to_string(std::numeric_limits<uint64_t>::max()));
	if (terrainDto.seaLevel < 0.0f)
		throw std::runtime_error("terrainGenerator: seaLevel can't be negative");
	if (terrainDto.seaLevel > static_cast<float>(CHUNK_SIZE_Y))
		throw std::runtime_error("terrainGenerator: seaLevel can't be larger than " + std::to_string(CHUNK_SIZE_Y));
}

TerrainGeneratorDTO	Settings::loadTerrainGenerator()
{
	TerrainGeneratorDTO terrainDto;
	std::string			filename = {};
	try
	{
		filename = _paths.at("terrainGenerator");
		JSON::Parser	terrainGeneratorJSON(filename);

		JSON::NodePtr	root = terrainGeneratorJSON.getRoot();
		terrainDto.seed = root->get("seed")->getNumber();
		terrainDto.seaLevel = root->get("seaLevel")->getNumber();
		terrainDto.caveDiameter = root->get("caveDiameter")->getNumber();

		JSON::NodePtr	NoiseSettings = root->get("noiseSettings");
		loadNoiseSettings(terrainDto.continentalness, NoiseSettings->get("continentalness"));
		loadNoiseSettings(terrainDto.cave, NoiseSettings->get("cave"));
		
		validateSettings(terrainDto);
		return (terrainDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + filename + ": " + std::string(e.what()));
	}
}