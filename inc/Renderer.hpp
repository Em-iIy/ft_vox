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
		void			swapFrameBuffer(int direction);

	private:
		void			initShaders();
		void			initMeshes();
		void			initFrameBuffers();
		void			initSsaoSamples();
		void			initSsaoNoise();

		void			cleanShaders();
		void			cleanMeshes();
		void			cleanFrameBuffers();

		void			shadowPass();
		void			terrainGeometryPass();
		void			waterGeometryPass();
		void			SSAOPass();
		void			terrainLightingPass();
		void			waterLightingPass();
		void			renderFinal();

		void			renderSun();
		void			renderUI();

		mlm::vec3		_bgColor;
		bool			_isUnderwater = false;

		float			_time = 0;
		bool			_pause = false;
		int				_lightingMode = 0;

		Shader			_quadShader;
		Shader			_shadowShader;
		Shader			_ssaoShader;
		Shader			_ssaoBlurShader;
		Shader			_geometryShader;
		Shader			_lightingShader;
		Shader			_waterShader;
		Shader			_cubeShader;
		Shader			_depthShader;

		Mesh			_cubeMesh;
		Mesh			_quadMesh;

		VoxEngine		&_engine;
		ChunkManager	&_manager;
		Camera			&_camera;

		FrameBuffer		_terrainGeometryFrameBuffer;
		FrameBuffer		_waterGeometryFrameBuffer;
		FrameBuffer		_shadowFrameBuffer;
		FrameBuffer		_terrainLightingFrameBuffer;
		FrameBuffer		_waterLightingFrameBuffer;
		FrameBuffer		_ssaoFrameBuffer;
		FrameBuffer		_ssaoBlurFrameBuffer;
		uint32_t		currentFrameBufferIdx = 0;
		std::vector<std::pair<GLuint, std::string>>	frameBufferIds;

		GLuint			_ssaoNoiseTex;

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