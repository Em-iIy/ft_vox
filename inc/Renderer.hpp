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
		mlm::mat4		&getLightProjection();
		mlm::mat4		&getLightView();
		mlm::vec3		&getSunPos();

		void			togglePause();
		void			setLightingMode(int mode);
	
	private:
		void			initShaders();
		void			initMeshes();
		void			initFrameBuffers();
		void			initSsaoSamples();

		void			cleanShaders();
		void			cleanMeshes();
		void			cleanFrameBuffers();

		void			renderChunks();
		void			updateChunkShader();
		void			renderShadowMap();
		void			renderTerrain();
		void			renderWater();
		void			renderWaterFinal();
		void			renderSun();
		void			renderFinal();
		
		void			renderUI();

		mlm::vec3		_bgColor;
		bool			_isUnderwater = false;

		float			_time;
		bool			_pause = false;
		int				_lightingMode = 0;

		Shader			_chunkShader;
		Shader			_cubeShader;
		Shader			_quadShader;
		Shader			_depthShader;
		Shader			_waterShader;
		Shader			_shadowShader;
		Shader			_ssaoShader;

		Mesh			_cubeMesh;
		Mesh			_quadMesh;

		VoxEngine		&_engine;
		ChunkManager	&_manager;
		Camera			&_camera;
		FrameBuffer		_geometryFrameBuffer;
		FrameBuffer		_waterFrameBuffer;
		FrameBuffer		_shadowFrameBuffer;
		FrameBuffer		_ssaoFrameBuffer;

		mlm::mat4		_projection;
		mlm::mat4		_view;

		mlm::mat4		_lightProjection;
		mlm::mat4		_lightView;
		mlm::vec3		_sunDir;
		mlm::vec3		_sunPos;

		void			updateProjection();
		void			updateView();
		void			updateLightProjection();
		void			updateLightView();
		void			updateUnderWater();
		void			updateSunPos();
		void			updateTime();

		// void	renderText();
};