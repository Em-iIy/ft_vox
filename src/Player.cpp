/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Player.hpp"

void		Player::setActiveBlock(Block::Type type)
{
	_activeBlock = type;
}

Block::Type	Player::getActiveBlock() const
{
	return (_activeBlock);
}
