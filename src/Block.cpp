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

mlm::vec3	Block::getTypeColor() const
{
	switch (_type)
	{
	case AIR:
		return (mlm::vec3(0.0f));
	case DIRT:
		return (mlm::vec3(0.7f, 0.2f, 0.2f));
	case GRASS:
		return (mlm::vec3(0.2f, 1.0f, 0.25f));
	case STONE:
		return (mlm::vec3(0.5f));
	case WATER:
		return (mlm::vec3(0.0f, 0.1f, 1.0f));
	}
	return (mlm::vec3(0.0f));
}


bool		Block::getEnabled() const
{
	return (_isEnabled);
}

void		Block::setEnabled(bool enabled)
{
	_isEnabled = enabled;
}
