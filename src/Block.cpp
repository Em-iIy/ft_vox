/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#include "Block.hpp"


Block::Block(): _type(AIR)
{}

Block::Block(Type type): _type(type)
{}

Block	&Block::operator=(const Block &src)
{
	_type = src._type;
	return (*this);
}


bool		Block::getEnabled() const
{
	return (_isEnabled);
}

void		Block::setEnabled(bool enabled)
{
	_isEnabled = enabled;
}

bool		Block::getTransparent() const
{
	return (_type == WATER || _type == AIR);
	// 	return (true);
	// return (false);
}

Block::Type		Block::getType() const
{
	return (_type);
}

void		Block::setType(Block::Type type)
{
	_type = type;
}
