/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

#include "ChunkManager.hpp"
#include "Camera.hpp"

class VoxEngine;

class Renderer {
	public:
		Renderer(VoxEngine &engine, ChunkManager &manager, Camera &camera);
		~Renderer();

		void			init();
		void			update();
		void			render();

		mlm::mat4		&getProjection();
		mlm::mat4		&getView();
	
	private:
		void			renderChunks();
		void			renderUI();

		mlm::vec3		_bgColor;
		bool			_isUnderwater = false;

		Shader			_chunkShader;
		Shader			_cubeShader;

		Mesh			_cubeMesh;

		VoxEngine		&_engine;
		ChunkManager	&_manager;
		Camera			&_camera;

		mlm::mat4		_projection;
		mlm::mat4		_view;

		void			updateProjection();
		void			updateView();
		void			updateUnderWater();

		// void	renderText();
};