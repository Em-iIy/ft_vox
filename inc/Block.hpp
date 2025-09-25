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
			WATER,
		};

		Block();
		Block(Type type);
		Block(const Block &src);

		Block		&operator=(const Block &src);

		bool		getEnabled() const;
		void		setEnabled(bool enabled);
		bool		getTransparent() const;
		Type		getType() const;
		void		setType(Type type);

	private:
		Type		_type = AIR;
		bool		_isEnabled = false;
};