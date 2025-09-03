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
#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "Frustum.hpp"

const mlm::ivec2	WINDOW_SIZE(1000, 1000);

class VoxEngine: public Window {
	public:
		VoxEngine();
		~VoxEngine();

		void			run();
	
		Camera			&getCamera();
		Input			&getInput();

		Atlas			_atlas; // make private later
		Camera			_camera;
		Frustum			_frustum;

	private:
		void			init();

		void			mainLoop();
		void			input();

		void			cleanup();


		Input			_input;
		ChunkManager	_chunkManager;
};