/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#include "Settings.hpp"

#include <vector>

std::map<std::string, std::string>	Settings::_paths;

const std::vector<std::string>			requiredPaths = {
	"terrainGenerator",
	"chunkManager",
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
