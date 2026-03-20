/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#pragma once

#include "TerrainGenerator.hpp"
#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "Sky.hpp"
#include "Camera.hpp"
#include "VoxEngine.hpp"
#include "json-parser/json-parser.hpp"

#include <map>

/*
	'Static' class that keeps track off all the different types of settings files
		with the associated paths.
	
	It provides loading functions that parse the settings files, and return the required
		DTO struct to then be loaded into their own resource.
*/
class Settings {
	public:
		Settings();

		static void					loadPaths(int argc, char **argv);

		static void					ensurePaths();

		static TerrainGeneratorDTO	loadTerrainGenerator();
		static ChunkManagerDTO		loadChunkManager();
		static AtlasDTO				loadAtlas();
		static SkyDTO				loadSky();
		static EngineDTO			loadEngine();

	private:
		// Setting filenames
		static std::map<std::string, std::string>	_paths;

};
