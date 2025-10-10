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
		void			cleanup();
		void			update();
		void			render();

		mlm::mat4		&getProjection();
		mlm::mat4		&getView();
	
	private:
		void			initShaders();
		void			initMeshes();
		void			initFrameBuffers();

		void			cleanShaders();
		void			cleanMeshes();
		void			cleanFrameBuffers();

		void			renderChunks();
		void			updateChunkShader();
		void			renderTerrain();
		void			renderWater();
		
		void			renderUI();

		mlm::vec3		_bgColor;
		bool			_isUnderwater = false;

		Shader			_chunkShader;
		Shader			_cubeShader;
		Shader			_waterShader;
		Shader			_quadShader;

		Mesh			_cubeMesh;
		Mesh			_quadMesh;

		VoxEngine		&_engine;
		ChunkManager	&_manager;
		Camera			&_camera;
		FrameBuffer		_waterFrameBuffer;

		mlm::mat4		_projection;
		mlm::mat4		_view;

		void			updateProjection();
		void			updateView();
		void			updateUnderWater();

		// void	renderText();
};