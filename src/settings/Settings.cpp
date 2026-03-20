/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#include "Settings.hpp"

#include <vector>

std::map<std::string, std::string>	Settings::_paths;

const std::vector<std::string>	requiredPaths = {
	"terrainGenerator",
	"chunkManager",
	"renderer",
	"atlas",
	"sky",
	"engine",
};

Settings::Settings() {}

void	Settings::loadPaths(int argc, char **argv)
{
	if (argc != 2)
		throw std::runtime_error("Settings: Missing settings file");
	try
	{
		// Load JSON from main settings file
		JSON::Parser	settingsJSON(argv[1]);

		// Loop through all the different paths inside the root JSON object
		JSON::NodePtr	settingsRoot = settingsJSON.getRoot();
		JSON::ObjectPtr	rootObj = settingsRoot->getObject();
		for (auto &[key, val] : *rootObj)
		{
			// Construct the paths and store in map
			_paths[key] = val->get("path")->getString() + val->get("name")->getString();
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
		// Query the list of required paths to see if they exist in the map from the settings file
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
