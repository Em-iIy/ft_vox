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

	private:
		void			_initShaders();
		void			_initMeshes();
		void			_initFrameBuffers();
		void			_initSsaoSamples();
		void			_initSsaoBlurShader();
		void			_initSsaoNoise();

		void			_cleanShaders();
		void			_cleanFrameBuffers();

		void			_shadowPass();
		void			_terrainGeometryPass();
		void			_waterGeometryPass();
		void			_SSAOPass();
		void			_terrainLightingPass();
		void			_waterLightingPass();
		void			_renderSky();
		void			_renderSkyColor();
		void			_renderSolarBodies();
		void			_renderAurora();
		void			_renderFinal();

		void			_renderUI();

		mlm::vec3		_bgColor;
		bool			_isUnderwater = false;

		Shader			_quadShader;
		Shader			_shadowShader;
		Shader			_ssaoShader;
		Shader			_ssaoBlurShader;
		Shader			_geometryShader;
		Shader			_lightingShader;
		Shader			_waterShader;
		Shader			_cubeShader;
		Shader			_depthShader;
		Shader			_skyShader;
		Shader			_solarBodiesShader;
		Shader			_auroraShader;

		Mesh			_cubeMesh;
		Mesh			_quadMesh;
		Mesh			_sphereMesh;

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
		FrameBuffer		_skyFrameBuffer;
		FrameBuffer		_auroraFrameBuffer;

		GLuint			_ssaoNoiseTex;

		mlm::mat4		_projection;
		mlm::mat4		_view;

		mlm::mat4		_lightProjection;
		mlm::mat4		_lightView;
		mlm::vec3		_sunDir;
		mlm::vec3		_sunPos;

		void			_updateProjection();
		void			_updateView();
		void			_updateLightProjection();
		void			_updateLightView();
		void			_updateUnderWater();
		void			_updateSunPos();
};
