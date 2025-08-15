/*
Created by: Emily (Em_iIy) Winnink
Created on: 06/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

class Block {
	public:

		enum Type {
			AIR,
			DIRT,
			GRASS,
			STONE,
		};

		Block();
		Block(const mlm::vec3 &color, Type type);
		Block	&operator=(const Block &src);

		mlm::vec3	_color = mlm::vec3(0.0f);
		Type		_type = AIR;
		bool		_isEnabled = false;

		mlm::vec3	getTypeColor() const;
		bool		getEnabled() const;
		void		setEnabled(bool enabled);
};