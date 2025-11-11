/*
Created by: Emily (Em_iIy) Winnink
Created on: 11/11/2025
*/

#include "Settings.hpp"

const std::unordered_map<std::string, Block::Type>	BlockStringType = {
	{"Block::AIR", Block::AIR},
	{"Block::DIRT", Block::DIRT},
	{"Block::GRASS", Block::GRASS},
	{"Block::STONE", Block::STONE},
	{"Block::WATER", Block::WATER}
};

const std::unordered_map<std::string, AtlasDTO::SIDE>	BlockSide = {
	{"TOP", AtlasDTO::SIDE::TOP},
	{"EAST", AtlasDTO::SIDE::EAST},
	{"WEST", AtlasDTO::SIDE::WEST},
	{"NORTH", AtlasDTO::SIDE::NORTH},
	{"SOUTH", AtlasDTO::SIDE::SOUTH},
	{"BOTTOM", AtlasDTO::SIDE::BOTTOM},
};

static void	loadTextureOffsets(std::unordered_map<std::string, mlm::vec2> &target, JSON::NodePtr node)
{
	JSON::Object	&obj = *node->getObject();

	for (auto [key, array] : obj)
	{
		mlm::vec2 temp = mlm::vec2((*array)[0]->getNumber(), (*array)[1]->getNumber());
		target[key] = temp;
	}
}

static void	loadBlock(std::vector<std::string> &target, JSON::NodePtr node)
{
	JSON::Object	&obj = *node->getObject();

	target.resize(6);
	for (auto [key, val] : obj)
	{
		try
		{
			target[static_cast<int>(BlockSide.at(key))] = val->getString();
		}
		catch(const std::exception& e)
		{
			throw std::runtime_error("invalid direction `" + key + "`");
		}
	}
}

static void	loadBlocks(std::unordered_map<Block::Type, std::vector<std::string>> &target, JSON::NodePtr node)
{
	JSON::Object	&obj = *node->getObject();

	for (auto [key, val] : obj)
	{
		std::vector<std::string>	temp;
		loadBlock(temp, val);
		try
		{
			target[BlockStringType.at(key)] = temp;
		}
		catch(const std::exception& e)
		{
			throw std::runtime_error("invalid type `" + key + "`");
		}
		
	}
}

static void	validateTextures(AtlasDTO &atlasDto)
{
	for (auto &[key, val] : atlasDto.blockOffsets)
	{
		for (const std::string &str : val)
		{
			if (!atlasDto.textureOffsets.contains(str))
			{
				throw std::runtime_error("missing texture `" + str + "`");
			}
		}
	}
}

AtlasDTO	Settings::loadAtlas()
{
	AtlasDTO	atlasDto;
	try
	{
		JSON::Parser	chunkManagerJSON(_paths.at("atlas"));

		JSON::NodePtr	root = chunkManagerJSON.getRoot();
		atlasDto.filename = root->get("atlas")->get("path")->getString() + root->get("atlas")->get("name")->getString();
		atlasDto.pixelWidth = root->get("pixelWidth")->getNumber();
		loadTextureOffsets(atlasDto.textureOffsets, root->get("textures"));
		loadBlocks(atlasDto.blockOffsets, root->get("blocks"));
		validateTextures(atlasDto);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("Settings: " + std::string(e.what()));
	}
	return (atlasDto);
}