/*
Created by: Emily (Em_iIy) Winnink
Created on: 27/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

#include "Block.hpp"

#include <unordered_map>



class Atlas
{
private:
	std::unordered_map<Block::Type, std::vector<mlm::vec2>>	_offsets;

public:
	Tex2d													_texture; // make bind function
	Atlas();
	bool	load(const std::string &atlasFileName, uint32_t len);
	~Atlas();

	const std::vector<mlm::vec2>	&getOffset(Block::Type blockType);
	void	del();
};
