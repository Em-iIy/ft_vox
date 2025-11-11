/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	VoxEngine	engine;

	try
	{
		Settings::loadPaths(argc, argv);
		[[ maybe_unused ]] AtlasDTO atlasDto = Settings::loadAtlas();
		engine.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return (0);
}