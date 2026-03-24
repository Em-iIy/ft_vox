/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include "json-parser/json-parser.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "Frustum.hpp"
#include "Renderer.hpp"
#include "Player.hpp"
#include "TerrainGenerator.hpp"
#include "Sky.hpp"

struct WindowSettings {
	float	width;
	float	height;
	bool	fullscreen;
	bool	vsync;
};

struct EngineDTO {
	CameraSettings	cameraSettings;
	WindowSettings	windowSettings;
};

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
		Frustum			&getShadowFrustum();
		Sky				&getSky();

		void			setFrustumUpdate();

	private:
		void			_init();
		void			_initWindow(EngineDTO &settings);
		void			_initInput();
		void			_initResources(EngineDTO &settings);
		void			_initComponents();

		void			_mainLoop();

		void			_update();
		void			_cleanup();

		void			_updateFrustum(const mlm::mat4 &projection, const mlm::mat4 &view);
		void			_updateShadowFrustum(const mlm::mat4 &projection, const mlm::mat4 &view);

		bool			_frustumUpdate = true;

		Input			_input;
		ChunkManager	_chunkManager;
		Atlas			_atlas;
		Camera			_camera;
		Frustum			_frustum;
		Frustum			_shadowFrustum;
		Renderer		_renderer;
		Player			_player;
		Sky				_sky;
};
