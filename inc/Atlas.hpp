/*
Created by: Emily (Em_iIy) Winnink
Created on: 27/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Block.hpp"

#include <unordered_map>


struct AtlasDTO {
	enum class SIDE {
		TOP = 0,
		EAST,
		WEST,
		NORTH,
		SOUTH,
		BOTTOM
	};
	std::unordered_map<std::string, mlm::vec2>					textureOffsets;
	std::string													filename;
	float														pixelWidth;
	std::unordered_map<Block::Type, std::vector<std::string>>	blockOffsets;
};

class Atlas {
	public:
		Atlas();
		~Atlas();

		bool													load();
		void													bind();
		const std::vector<mlm::vec2>							&getOffset(Block::Type blockType);
		const std::vector<mlm::vec2>							&getCorners();
		void													del();
		
	private:
		std::unordered_map<Block::Type, std::vector<mlm::vec2>>	_offsets;
		std::vector<mlm::vec2>									_uvCorners;
		Tex2d													_texture; // make bind function
};
