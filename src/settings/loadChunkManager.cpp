/*
Created by: Emily (Em_iIy) Winnink
Created on: 22/10/2025
*/

#include "Settings.hpp"

ChunkManagerDTO	Settings::loadChunkManager()
{
	ChunkManagerDTO chunkManagerDto;
	try
	{
		JSON::Parser	chunkManagerJSON(_paths.at("chunkManager"));

		JSON::NodePtr	root = chunkManagerJSON.getRoot();
		chunkManagerDto.renderDistance = root->get("renderDistance")->getNumber();
		chunkManagerDto.threadCount = root->get("threadCount")->getNumber();
		chunkManagerDto.maxLoad = root->get("maxLoad")->getNumber();
		chunkManagerDto.maxGenerate = root->get("maxGenerate")->getNumber();
		chunkManagerDto.maxMesh = root->get("maxMesh")->getNumber();

		if ((chunkManagerDto.maxMesh < 1)
			|| (chunkManagerDto.maxGenerate < 1)
			|| (chunkManagerDto.maxLoad < 1)
			|| (chunkManagerDto.threadCount < 1)
			|| (chunkManagerDto.renderDistance < 1))
			throw std::runtime_error("chunkManager settings can't be smaller than 1");
		return (chunkManagerDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
}