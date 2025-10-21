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

TerrainGeneratorDTO	Settings::loadTerrainGenerator()
{
	try
	{
		JSON::Parser	terrainGeneratorJSON(_paths.at("terrainGenerator"));

		std::cout << terrainGeneratorJSON.getRoot()->stringify() << std::endl;
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
	
	TerrainGeneratorDTO terrainDto = {
		.seed = 4242,
		.seaLevel = 110,
		.caveDiameter = 0.02f,
		.continentalness = {
			.spline = Spline({
				mlm::vec2(-1.0f, 250.0f),
				mlm::vec2(-0.8f, 95.0f),
				mlm::vec2(-0.2f, 95.0f),
				mlm::vec2(0.0f, 108.0f),
				mlm::vec2(0.25f, 120.0f),
				mlm::vec2(0.45f, 180.0f),
				mlm::vec2(1.00f, 200.0f)
			}),
			.depth = 5,
			.step = 2.0,
			.zoom = 400,
			.dimensions = 2,
		},
		.cave = {
			.spline = Spline({
				mlm::vec2(-1.0f, -1.0f),
				mlm::vec2(1.0f, 1.0f),
			}),
			.depth = 1,
			.step = 1.0,
			.zoom = 160,
			.dimensions = 3,
		},
	};
	return (terrainDto);
}