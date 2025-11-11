/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#pragma once

#include "TerrainGenerator.hpp"
#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "json-parser/json-parser.hpp"

#include <map>

class Settings {
	public:
		Settings();

		static void					loadPaths(int argc, char **argv);

		static void					ensurePaths();

		static TerrainGeneratorDTO	loadTerrainGenerator();
		static ChunkManagerDTO		loadChunkManager();
		static AtlasDTO				loadAtlas();

	private:
		// Setting filenames
		static	std::map<std::string, std::string>	_paths;

};