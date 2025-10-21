/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#include "Settings.hpp"

#include <vector>

std::map<std::string, std::string>	Settings::_paths;

const std::vector<std::string>			requiredPaths = {
	"terrainGenerator",
	"renderer",
	"atlas",
};

Settings::Settings() {}

void	Settings::loadPaths(int argc, char **argv)
{
	if (argc != 2)
		throw std::runtime_error("Settings: Missing settings file");
	try
	{
		JSON::Parser	settingsJSON(argv[1]);

		JSON::NodePtr	settingsRoot = settingsJSON.getRoot();
		JSON::ObjectPtr	rootObj = settingsRoot->getObject();
		for (auto &[key, val] : *rootObj)
		{
			_paths[key] = val->get("path")->getString();
		}
		ensurePaths();
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
}

void	Settings::ensurePaths()
{
	bool	complete = true;
	if (_paths.size() < requiredPaths.size())
		complete = false;
	else
	{
		for (const std::string &path : requiredPaths)
		{
			if (!_paths.contains(path))
			{
				complete = false;
				break ;
			}
		}
	}
	if (!complete)
	{
		std::string msg = "Missing paths - Make sure to include: ";
		for (const std::string &path : requiredPaths)
			msg += "`" + path + "` ";
		throw std::runtime_error(msg);
	}
}

static void	loadSpline(Spline &target, JSON::NodePtr node)
{
	JSON::ListPtr			array = node->getList();
	std::size_t				len = array->size();
	std::vector<mlm::vec2>	points;

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
	target.dimensions = node->get("dimensions")->getNumber();
}

TerrainGeneratorDTO	Settings::loadTerrainGenerator()
{
	TerrainGeneratorDTO terrainDto;
	try
	{
		JSON::Parser	terrainGeneratorJSON(_paths.at("terrainGenerator"));

		JSON::NodePtr	root = terrainGeneratorJSON.getRoot();
		terrainDto.seed = root->get("seed")->getNumber();
		terrainDto.seaLevel = root->get("seaLevel")->getNumber();
		terrainDto.caveDiameter = root->get("caveDiameter")->getNumber();

		JSON::NodePtr	NoiseSettings = root->get("noiseSettings");
		loadNoiseSettings(terrainDto.continentalness, NoiseSettings->get("continentalness"));
		loadNoiseSettings(terrainDto.cave, NoiseSettings->get("cave"));
		// std::cout << root->stringify() << std::endl;
		return (terrainDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
}