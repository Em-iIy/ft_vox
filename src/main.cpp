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

	Settings::loadPaths(argc, argv);

	TerrainGenerator	terrain(Settings::loadTerrainGenerator());

	engine.run();
	return (0);
}