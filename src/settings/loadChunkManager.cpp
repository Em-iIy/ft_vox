/*
Created by: Emily (Em_iIy) Winnink
Created on: 22/10/2025
*/

#include "Settings.hpp"


static void	validateSettings(const ChunkManagerDTO &chunkManagerDto)
{
	const float LOWER_LIMIT = 1.0f;
	const float UPPER_LIMIT = 256.0f;
	if ((chunkManagerDto.maxMesh < LOWER_LIMIT)
		|| (chunkManagerDto.maxGenerate < LOWER_LIMIT)
		|| (chunkManagerDto.maxLoad < LOWER_LIMIT)
		|| (chunkManagerDto.threadCount < LOWER_LIMIT)
		|| (chunkManagerDto.renderDistance < LOWER_LIMIT))
		throw std::runtime_error("chunkManager settings can't be smaller than " + std::to_string(static_cast<int>(LOWER_LIMIT)));
	if ((chunkManagerDto.maxMesh > UPPER_LIMIT)
		|| (chunkManagerDto.maxGenerate > UPPER_LIMIT)
		|| (chunkManagerDto.maxLoad > UPPER_LIMIT)
		|| (chunkManagerDto.threadCount > UPPER_LIMIT)
		|| (chunkManagerDto.renderDistance > UPPER_LIMIT))
		throw std::runtime_error("chunkManager settings can't be larger than " + std::to_string(static_cast<int>(UPPER_LIMIT)));
}

ChunkManagerDTO	Settings::loadChunkManager()
{
	ChunkManagerDTO chunkManagerDto;
	std::string		filename = {};
	try
	{
		filename = _paths.at("chunkManager");
		JSON::Parser	chunkManagerJSON(filename);

		JSON::NodePtr	root = chunkManagerJSON.getRoot();
		chunkManagerDto.renderDistance = root->get("renderDistance")->getNumber();
		chunkManagerDto.threadCount = root->get("threadCount")->getNumber();
		chunkManagerDto.maxLoad = root->get("maxLoad")->getNumber();
		chunkManagerDto.maxGenerate = root->get("maxGenerate")->getNumber();
		chunkManagerDto.maxMesh = root->get("maxMesh")->getNumber();

		validateSettings(chunkManagerDto);
		return (chunkManagerDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + filename + ": " + std::string(e.what()));
	}
}