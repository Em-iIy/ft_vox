/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#include "Settings.hpp"

Settings::Settings(int argc, char **argv)
{
	if (argc != 2)
		throw std::runtime_error("Settings: Missing settings file");
	try
	{
		JSON::Parser	settingsJSON(argv[1]);

		JSON::NodePtr	settingsRoot = settingsJSON.getRoot();
		_terrainGenerator = settingsRoot->get("terrainGenerator")->get("path")->getString();
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
	
}

Settings::~Settings()
{

}

const std::string	&Settings::getTerrainGeneratorFilename() const
{
	return (_terrainGenerator);
}

TerrainGeneratorDTO	Settings::loadTerrainGenerator(const std::string &filename)
{
	try
	{
		JSON::Parser	terrainGeneratorJSON(filename);

		std::cout << terrainGeneratorJSON.getRoot()->stringify() << std::endl;
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
	
	TerrainGeneratorDTO terrainDto = {
		.seed = 4242,
		.seaLevel = 110,
		.caveDiameter = 0.2f,
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