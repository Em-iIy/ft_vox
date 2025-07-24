/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

const mlm::ivec2	WINDOW_SIZE(800, 600);

class VoxEngine {
	public:
		void	run();
	
	private:
		void	init();
		void	mainLoop();
		void	cleanup();

		Window		_window;
};