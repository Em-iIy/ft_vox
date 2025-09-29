/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#pragma once

#include "Block.hpp"

class Player {
	public:
		void		setActiveBlock(Block::Type type);
		Block::Type	getActiveBlock() const;

	private:
		Block::Type	_activeBlock = Block::STONE;
};