/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Chunk.hpp"

const mlm::ivec2	WINDOW_SIZE(1000, 1000);

class VoxEngine: public Window {
	public:
		void	run();
	
		Camera	&getCamera();
		Input	&getInput();

	private:
		void	init();

		void	mainLoop();
		void	input();

		void	cleanup();


		Camera		_camera;
		Input		_input;
};