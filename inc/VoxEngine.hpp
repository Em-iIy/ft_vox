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
#include "Renderer.hpp"
#include "Player.hpp"

const mlm::ivec2	WINDOW_SIZE(1000, 1000);

class VoxEngine: public Window {
	public:
		VoxEngine();
		~VoxEngine();

		void			run();
	
		Camera			&getCamera();
		Input			&getInput();
		ChunkManager	&getManager();
		Atlas			&getAtlas();
		Frustum			&getFrustum();

		void			setFrustumUpdate();


	private:
		void			init();

		void			mainLoop();
		void			input();

		void			cleanup();

		void			updateFrustum(const mlm::mat4 &projection, const mlm::mat4 &view);

		bool			_updateFrustum = true;

		Input			_input;
		ChunkManager	_chunkManager;
		Atlas			_atlas;
		Camera			_camera;
		Frustum			_frustum;
		Renderer		_renderer;
		Player			_player;
};