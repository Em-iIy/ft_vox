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

		return (chunkManagerDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
}