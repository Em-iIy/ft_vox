/*
Created by: Emily (Em_iIy) Winnink
Created on: 21/10/2025
*/

#pragma once

#include "TerrainGenerator.hpp"
#include "json-parser/json-parser.hpp"

class Settings {
	public:
		Settings(int argc, char **argv);
		~Settings();

		const std::string			&getTerrainGeneratorFilename() const;

		static TerrainGeneratorDTO	loadTerrainGenerator(const std::string &filename);

	private:
		// Setting filenames
		std::string					_terrainGenerator;

};