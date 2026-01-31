/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"

const mlm::vec3	SKY_COLOR_DAY(0.4f, 0.7f, 0.9f);
const mlm::vec3	SKY_COLOR_NIGHT(0.05f, 0.1f, 0.2f);
const mlm::vec3	WATER_COLOR(0.0f, 0.0f, 0.8f);

// const float		FOG_WATER_NEAR = 0.0f;
// const float		FOG_NEAR = 120.0f;
// const float		FOG_FAR = 160.0f;

const float		CLIPPING_NEAR = 0.25f;
const float		CLIPPING_FAR = 640.0f;

Renderer::Renderer(VoxEngine &engine, ChunkManager &manager, Camera &camera): _engine(engine), _manager(manager), _camera(camera)
{}

Renderer::~Renderer()
{}

void	Renderer::init()
{
	initShaders();
	initMeshes();
	initFrameBuffers();
	initSsaoSamples();
	initSsaoNoise();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void	Renderer::initShaders()
{
	// Draws textured quad to the screen
	_quadShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/quad.frag");

	// Render shadow map
	_shadowShader = Shader("./resources/shaders/shadow.vert", "./resources/shaders/shadow.frag");

	// SSAO and blur shaders
	_ssaoShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/SSAO.frag");
	_ssaoBlurShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/SSAOBlur.frag");

	// Draw basic geometry to gBuffers
	_geometryShader = Shader("./resources/shaders/geometry.vert", "./resources/shaders/geometry.frag");

	// Final lighting shader - Combines Light with shadows and SSAO
	_lightingShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/lighting.frag");

	// Draws water with opacity
	_waterShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/water.frag");

	// Shader for the skybox
	_skyShader = Shader("./resources/shaders/sky.vert", "./resources/shaders/sky.frag");

	// Shader for the skybox
	_solarBodiesShader = Shader("./resources/shaders/sky.vert", "./resources/shaders/solarBodies.frag");

	// General UI/Debug shaders
	_cubeShader = Shader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");
	_depthShader = Shader("./resources/shaders/depth.vert", "./resources/shaders/depth.frag");
}

void	Renderer::initMeshes()
{
	std::vector<Vertex>		cubeVertices = {
		{mlm::vec3(-1.0f, -1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, -1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(-1.0f, 1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(-1.0f, -1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, -1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(-1.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
	};
	std::vector<uint32_t>	cubeIndices = {
		0, 2, 1,
		1, 2, 3,
		4, 5, 6,
		5, 7, 6,
		0, 4, 6,
		0, 6, 2,
		1, 7, 5,
		1, 3, 7,
		2, 6, 7,
		2, 7, 3,
		1, 4, 0,
		1, 5, 4,
	};
	_cubeMesh = Mesh(cubeVertices, cubeIndices);

	std::vector<Vertex>		quadVertices = {
		{mlm::vec3(-1.0f, -1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f, 0.0f)},
		{mlm::vec3(1.0f, -1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(1.0f, 0.0f)},
		{mlm::vec3(-1.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f, 1.0f)},
		{mlm::vec3(1.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(1.0f, 1.0f)}
	};
	std::vector<uint32_t>	quadIndices = {
		0, 1, 3,
		0, 3, 2
	};
	_quadMesh = Mesh(quadVertices, quadIndices);

	std::vector<Vertex>		sphereVertices;
	std::vector<uint32_t>	sphereIndices;

	constexpr float	PI = M_PI;
	constexpr float	PI_2 = 2.0f * PI;
	const int		stacks = 64;
	const int		slices = 64;
	const float		radius = 1.0f;

	for (int i = 0; i <= stacks; ++i)
	{
		float	stack = static_cast<float>(i) / stacks;
		float	phi = stack * PI;

		for (int j = 0; j <= slices; ++j)
		{
			float	slice = static_cast<float>(j) / slices;
			float	theta = slice * PI_2;

			mlm::vec3	pos(
				radius * std::sin(phi) * std::cos(theta),
				radius * std::cos(phi),
				radius * std::sin(phi) * std::sin(theta)
			);

			Vertex vert;
			vert.pos = pos;
			vert.normal = mlm::normalize(mlm::vec3(0.0f) - pos);
			vert.texUV = mlm::vec2(slice, stack);

			sphereVertices.push_back(vert);
		}
	}

	const uint32_t stride = slices + 1;
	for (int i = 0; i < stacks; ++i)
	{
		for (int j = 0; j < slices; ++j)
		{
			uint32_t a = i * stride + j;
			uint32_t b = a + stride;
			uint32_t c = a + 1;
			uint32_t d = b + 1;

			sphereIndices.push_back(a);
			sphereIndices.push_back(b);
			sphereIndices.push_back(c);

			sphereIndices.push_back(b);
			sphereIndices.push_back(c);
			sphereIndices.push_back(d);
		}
	}
	_sphereMesh = Mesh(sphereVertices, sphereIndices);
}

void	Renderer::initFrameBuffers()
{
	frameBufferIds.emplace_back(0, "Default");

	mlm::ivec2	size = _engine.get_size();
	_terrainGeometryFrameBuffer.create(size.x, size.y);
	_terrainGeometryFrameBuffer.bind();
	_terrainGeometryFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_terrainGeometryFrameBuffer.attachColorTexture(1, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_terrainGeometryFrameBuffer.attachColorTexture(2, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_terrainGeometryFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_terrainGeometryFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_terrainGeometryFrameBuffer.unbind();
	if (_terrainGeometryFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Terrain Framebuffer missing");
	frameBufferIds.emplace_back(_terrainGeometryFrameBuffer.getColorTexture(0), "Geometry Color");
	frameBufferIds.emplace_back(_terrainGeometryFrameBuffer.getColorTexture(1), "Geometry Normal");
	frameBufferIds.emplace_back(_terrainGeometryFrameBuffer.getColorTexture(2), "Geometry Position");

	_waterGeometryFrameBuffer.create(size.x, size.y);
	_waterGeometryFrameBuffer.bind();
	_waterGeometryFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_waterGeometryFrameBuffer.attachColorTexture(1, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_waterGeometryFrameBuffer.attachColorTexture(2, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_waterGeometryFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_waterGeometryFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_waterGeometryFrameBuffer.unbind();
	if (_waterGeometryFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	frameBufferIds.emplace_back(_waterGeometryFrameBuffer.getColorTexture(0), "Water Color");
	frameBufferIds.emplace_back(_waterGeometryFrameBuffer.getColorTexture(1), "Water Normal");
	frameBufferIds.emplace_back(_waterGeometryFrameBuffer.getColorTexture(2), "Water Position");

	_terrainLightingFrameBuffer.create(size.x, size.y);
	_terrainLightingFrameBuffer.bind();
	_terrainLightingFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_terrainLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_terrainLightingFrameBuffer.unbind();
	if (_terrainLightingFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	frameBufferIds.emplace_back(_terrainLightingFrameBuffer.getColorTexture(0), "Terrain Lighting Color");
	
	_waterLightingFrameBuffer.create(size.x, size.y);
	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_waterLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_waterLightingFrameBuffer.unbind();
	if (_waterLightingFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	frameBufferIds.emplace_back(_waterLightingFrameBuffer.getColorTexture(0), "Water Lighting Color");

	_shadowFrameBuffer.create(std::max(size.x * 2, 4096), std::max(size.y * 2, 4096));
	_shadowFrameBuffer.bind();
	_shadowFrameBuffer.ensureDepthTexture(GL_DEPTH_COMPONENT, GL_FLOAT, true, GL_CLAMP_TO_BORDER, mlm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_shadowFrameBuffer.setDrawBuffers({});
	_shadowFrameBuffer.unbind();
	if (_shadowFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Shadow Framebuffer missing");

	_ssaoFrameBuffer.create(size.x, size.y);
	_ssaoFrameBuffer.bind();
	_ssaoFrameBuffer.attachColorTexture(0, GL_RED, GL_RED, GL_FLOAT, true, true, false);
	_ssaoFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	if (_ssaoFrameBuffer.checkStatus() == false)
		throw std::runtime_error("SSAO Framebuffer missing");
	frameBufferIds.emplace_back(_ssaoFrameBuffer.getColorTexture(0), "SSAO Color");

	_ssaoBlurFrameBuffer.create(size.x, size.y);
	_ssaoBlurFrameBuffer.bind();
	_ssaoBlurFrameBuffer.attachColorTexture(0, GL_RED, GL_RED, GL_FLOAT, true, true, false);
	_ssaoBlurFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	if (_ssaoBlurFrameBuffer.checkStatus() == false)
		throw std::runtime_error("SSAO Blur Framebuffer missing");
	frameBufferIds.emplace_back(_ssaoBlurFrameBuffer.getColorTexture(0), "SSAO Blur Color");
}

void	Renderer::initSsaoSamples()
{
	std::array<mlm::vec3, 8>		ssaoSamples;

	rng::fgen	gen = rng::generator(0.0f, 1.0f);

	for (uint32_t i = 0; i < ssaoSamples.size(); i++)
	{
		// Create sample in hemisphere
		mlm::vec3 sample(
			rng::rand(gen) * 2.0f - 1.0f,
			rng::rand(gen) * 2.0f - 1.0f,
			rng::rand(gen)	
		);
		sample = mlm::normalize(sample);
		sample *= rng::rand(gen);

		// focus more samples closer to the center
		float scale = static_cast<float>(i) / static_cast<float>(ssaoSamples.size());
		scale = std::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		ssaoSamples[i] = sample;
	}
	_ssaoShader.use();
	_ssaoShader.set_int("uSampleCount", ssaoSamples.size());

	// replace with ssbo
	for (uint32_t i = 0; i < ssaoSamples.size(); i++)
		_ssaoShader.set_vec3("uSamples[" + std::to_string(i) + "]", ssaoSamples[i]);

	_ssaoShader.set_float("uRadius", 0.8f);

	_ssaoShader.set_int("uGNormal", 0);
	_ssaoShader.set_int("uGPosition", 1);
	_ssaoShader.set_int("uNoiseTex", 2);

	_ssaoBlurShader.use();
	_ssaoBlurShader.set_int("uTexture", 0);
}

void	Renderer::initSsaoNoise()
{
	std::array<mlm::vec3, 16>	ssaoNoise;

	rng::fgen	gen = rng::generator(0.0f, 1.0f);

	for (uint32_t i = 0; i < ssaoNoise.size(); i++)
	{
		mlm::vec3 noise(
			rng::rand(gen) * 2.0f - 1.0f,
			rng::rand(gen) * 2.0f - 1.0f,
			0.0f	
		);
		ssaoNoise[i] = noise;
	}

	glGenTextures(1, &_ssaoNoiseTex);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void	Renderer::cleanup()
{
	cleanShaders();
	cleanMeshes();
	cleanFrameBuffers();
}

void	Renderer::cleanShaders()
{
	_quadShader.del();
	_shadowShader.del();
	_ssaoShader.del();
	_ssaoBlurShader.del();
	_geometryShader.del();
	_lightingShader.del();
	_waterShader.del();
	_cubeShader.del();
	_depthShader.del();
	_skyShader.del();
	_solarBodiesShader.del();
}

void	Renderer::cleanMeshes()
{
	// _cubeMesh.del()
	// _quadMesh.del()
	// _sphereMesh.del()
}

void	Renderer::cleanFrameBuffers()
{
	_terrainGeometryFrameBuffer.destroy();
	_waterGeometryFrameBuffer.destroy();
	_shadowFrameBuffer.destroy();
	_terrainLightingFrameBuffer.destroy();
	_waterLightingFrameBuffer.destroy();
	_ssaoFrameBuffer.destroy();
	_ssaoBlurFrameBuffer.destroy();

	// Clear all framebuffers ids that were stored for debug purposes
	frameBufferIds.clear();
}

void	Renderer::update()
{
	updateTime();
	updateProjection();
	updateView();
	updateLightProjection();
	updateLightView();
	updateUnderWater();
	updateSunPos();
}

void	Renderer::render()
{
	FrameBuffer::unbind();
	FrameBuffer::clear(true, true, mlm::vec4(_bgColor, 0.0f));

	shadowPass();
	terrainGeometryPass();
	waterGeometryPass();
	SSAOPass();
	terrainLightingPass();
	waterLightingPass();
	renderSkyBox();
	renderFinal();

	return ;

	renderSun();
	renderUI();
}

void	Renderer::shadowPass()
{
	_shadowShader.use();
	_shadowShader.set_mat4("uLightProjection", _lightProjection);
	_shadowShader.set_mat4("uLightView", _lightView);

	_shadowFrameBuffer.bind();
	FrameBuffer::clear(false, true, mlm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glViewport(0, 0, _shadowFrameBuffer.getWidth(), _shadowFrameBuffer.getHeight());
	glDisable(GL_CULL_FACE);
	_manager.renderChunksShadows(_shadowShader);
	glEnable(GL_CULL_FACE);
	mlm::ivec2	size = _engine.get_size();
	glViewport(0, 0, size.x, size.y);
}

void	Renderer::terrainGeometryPass()
{
	_geometryShader.use();

	_geometryShader.set_mat4("uProjection", _projection);
	_geometryShader.set_mat4("uView", _view);
	
	glActiveTexture(GL_TEXTURE0);
	_engine.getAtlas().bind();
	_geometryShader.set_int("uAtlas", 0);

	_terrainGeometryFrameBuffer.bind();
	FrameBuffer::clearBufferfv(GL_COLOR, 0, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 1, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 2, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clear(false, true, mlm::vec4(0.0f));


	bool wireFrameMode = _engine.getInput().getWireFrameMode();
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	_manager.renderChunks(_geometryShader);

	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void	Renderer::waterGeometryPass()
{
	_geometryShader.use();

	_geometryShader.set_mat4("uProjection", _projection);
	_geometryShader.set_mat4("uView", _view);
	
	glActiveTexture(GL_TEXTURE0);
	_engine.getAtlas().bind();
	_geometryShader.set_int("uAtlas", 0);

	_waterGeometryFrameBuffer.bind();
	FrameBuffer::clearBufferfv(GL_COLOR, 0, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 1, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 2, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clear(false, true, mlm::vec4(0.0f));

	mlm::ivec2	size = _engine.get_size();
	_waterGeometryFrameBuffer.blitDepthFrom(_terrainGeometryFrameBuffer.getId(), size.x, size.y);
	_waterGeometryFrameBuffer.bind();

	bool wireFrameMode = _engine.getInput().getWireFrameMode();
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDisable(GL_CULL_FACE);
	_manager.renderWater(_geometryShader);
	glEnable(GL_CULL_FACE);

	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void	Renderer::SSAOPass()
{
	_ssaoShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _terrainGeometryFrameBuffer.getColorTexture(1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _terrainGeometryFrameBuffer.getColorTexture(2));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTex);

	mlm::ivec2	size = _engine.get_size();
	_ssaoShader.set_float("uScreenWidth", static_cast<float>(size.x));
	_ssaoShader.set_float("uScreenHeight", static_cast<float>(size.y));

	_ssaoShader.set_mat4("uProjection", _projection);

	_ssaoFrameBuffer.bind();
	FrameBuffer::clear(true, false, mlm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_quadMesh.draw(_ssaoShader);

	_ssaoBlurShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ssaoFrameBuffer.getColorTexture(0));
	_ssaoBlurFrameBuffer.bind();
	FrameBuffer::clear(true, false, mlm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_quadMesh.draw(_ssaoBlurShader);
}

void	Renderer::terrainLightingPass()
{
	_lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _terrainGeometryFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uGColor", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _terrainGeometryFrameBuffer.getColorTexture(1));
	_lightingShader.set_int("uGNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _terrainGeometryFrameBuffer.getColorTexture(2));
	_lightingShader.set_int("uGPosition", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());
	_lightingShader.set_int("uShadowMap", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uSSAO", 4);

	// _lightingShader.set_vec3("uLightPos", _sunDir)
	_lightingShader.set_vec3("uLightDir", _sunDir);
	_lightingShader.set_mat4("uLightView", _lightView);
	_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	_lightingShader.set_mat4("uView", _view);

	_lightingShader.set_bool("uIsWater", false);

	_terrainLightingFrameBuffer.bind();
	_terrainLightingFrameBuffer.clear(true, true, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	glDisable(GL_DEPTH_TEST);
	_quadMesh.draw(_lightingShader);
	glEnable(GL_DEPTH_TEST);
}

void	Renderer::waterLightingPass()
{
	_lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterGeometryFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uGColor", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _waterGeometryFrameBuffer.getColorTexture(1));
	_lightingShader.set_int("uGNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _waterGeometryFrameBuffer.getColorTexture(2));
	_lightingShader.set_int("uGPosition", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());
	_lightingShader.set_int("uShadowMap", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uSSAO", 4);

	// _lightingShader.set_vec3("uLightPos", _sunDir)
	_lightingShader.set_vec3("uLightDir", _sunDir);
	_lightingShader.set_mat4("uLightView", _lightView);
	_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	_lightingShader.set_mat4("uView", _view);

	_lightingShader.set_bool("uIsWater", true);

	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.clear(true, true, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	glDisable(GL_DEPTH_TEST);
	_quadMesh.draw(_lightingShader);
	glEnable(GL_DEPTH_TEST);
}

void	Renderer::renderSkyBox()
{
	FrameBuffer::unbind();
	_skyShader.use();
	_skyShader.set_mat4("uProjection", _projection);
	_skyShader.set_mat4("uView", _view);

	_skyShader.set_vec3("uSunDir", _sunDir);

	Sky &sky = _engine.getSky();
	float timePercent = sky.getTimePercent();

	_skyShader.set_vec4("uStops", mlm::vec4(0.38f, 0.47f, 0.61f, 1.0f));
	_skyShader.set_vec4("uColors[0]", sky._gradientStop0.sampleAt(timePercent));
	_skyShader.set_vec4("uColors[1]", sky._gradientStop1.sampleAt(timePercent));
	_skyShader.set_vec4("uColors[2]", sky._gradientStop2.sampleAt(timePercent));
	_skyShader.set_vec4("uColors[3]", sky._gradientStop3.sampleAt(timePercent));
	_skyShader.set_int("uStopCount", 4);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	_sphereMesh.draw(_skyShader);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	renderSun();
}

void	Renderer::renderSun()
{
	FrameBuffer::unbind();

	Sky &sky = _engine.getSky();
	
	_solarBodiesShader.use();
	_solarBodiesShader.set_mat4("uProjection", _projection);
	_solarBodiesShader.set_mat4("uView", _view);

	_solarBodiesShader.set_vec3("uSunDir", _sunDir);

	_solarBodiesShader.set_vec4("uSunDiskColor", sky._sun.diskColor);
	_solarBodiesShader.set_float("uSunDiskFactor", sky._sun.diskFactor);
	_solarBodiesShader.set_float("uSunDiskSize", sky._sun.diskSize);
	_solarBodiesShader.set_vec4("uSunGlowColor", sky._sun.glowColor);
	_solarBodiesShader.set_float("uSunGlowFactor", sky._sun.glowFactor);
	_solarBodiesShader.set_float("uSunGlowSharpness", sky._sun.glowShaprness);

	_solarBodiesShader.set_vec4("uMoonDiskColor", sky._moon.diskColor);
	_solarBodiesShader.set_float("uMoonDiskFactor", sky._moon.diskFactor);
	_solarBodiesShader.set_float("uMoonDiskSize", sky._moon.diskSize);
	_solarBodiesShader.set_vec4("uMoonGlowColor", sky._moon.glowColor);
	_solarBodiesShader.set_float("uMoonGlowFactor", sky._moon.glowFactor);
	_solarBodiesShader.set_float("uMoonGlowSharpness", sky._moon.glowShaprness);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	_sphereMesh.draw(_solarBodiesShader);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void	Renderer::renderFinal()
{
	FrameBuffer::unbind();
	glDisable(GL_DEPTH_TEST);

	_quadShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _terrainLightingFrameBuffer.getColorTexture(0));
	_quadShader.set_int("uRenderTex", 0);
	_quadMesh.draw(_quadShader);

	_waterShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterLightingFrameBuffer.getColorTexture(0));
	_waterShader.set_int("uRenderTex", 0);
	_waterShader.set_float("uWaterOpacity", 0.7f);
	_quadMesh.draw(_waterShader);

	glEnable(GL_DEPTH_TEST);
	// _quadShader.use();
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, frameBufferIds[currentFrameBufferIdx].first);
	// _quadShader.set_int("uRenderTex", 0);

	// _quadMesh.draw(_quadShader);
}

void	Renderer::renderUI()
{
	Expected<mlm::ivec3, bool>	rayWorldCoord = _manager.castRayIncluding();
	if (rayWorldCoord.hasValue())
	{
		_cubeShader.use();
		_cubeShader.set_mat4("uProjection", _projection);
		_cubeShader.set_mat4("uView", _view);
		mlm::mat4	model(1.0f);
		mlm::vec3	pos = static_cast<mlm::vec3>(rayWorldCoord.value()) - _camera.getPos();
		model = mlm::translate(model, pos);
		_cubeShader.set_mat4("uModel", model);

		_cubeShader.set_vec3("uColor", mlm::vec3(0.0f));
		_cubeShader.set_float("uAlpha", 0.2f);

		_cubeMesh.draw(_cubeShader);
	}

	{
		_depthShader.use();

		mlm::mat4	proj(1.0f);
		_depthShader.set_mat4("uProjection", proj);

		mlm::mat4	view(1.0f);
		_depthShader.set_mat4("uView", view);

		mlm::mat4	model(1.0f);
		model = mlm::translate(model, mlm::vec3(0.8f, 0.8f, -0.5f));
		model = mlm::scale(model, mlm::vec3(0.2f));
		_depthShader.set_mat4("uModel", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());
		_depthShader.set_int("uTexture", 0);

		bool wireFrameMode = _engine.getInput().getWireFrameMode();
		if (wireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_quadMesh.draw(_depthShader);
		if (wireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

}

void	Renderer::updateProjection()
{
	mlm::vec2	size = static_cast<mlm::vec2>(_engine.get_size());
	_projection = mlm::perspective(_camera.getZoom(), size.x / size.y, CLIPPING_NEAR, CLIPPING_FAR);
}

void	Renderer::updateView()
{
	_view = _camera.getViewMatrix();
}

void	Renderer::updateLightProjection()
{
	_lightProjection = mlm::ortho(-160.0f, 160.0f, -160.0f, 160.0f, 0.1f, 512.0f);
}

void	Renderer::updateLightView()
{
	_lightView = mlm::lookat(_sunPos, mlm::vec3(0.0f), mlm::vec3(0.0f, 1.0f, 0.0f));
}

mlm::vec3	v3Lerp(const mlm::vec3 &v1, const mlm::vec3 &v2, float t)
{
	return (v1 + t * (v2 - v1));
}

void	Renderer::updateUnderWater()
{
	auto	block = _manager.getBlockType(_camera.getPos());
	if (block.hasValue() && block.value() == Block::WATER)
	{
		_bgColor = WATER_COLOR;
		_isUnderwater = true;
	}
	else
	{
		_bgColor = v3Lerp(SKY_COLOR_NIGHT, SKY_COLOR_DAY, (sinf(_time) + 1.0) / 2);
		_isUnderwater = false;
	}
}

void	Renderer::updateSunPos()
{
	float	skyTime = _engine.getSky().getTimePercent() * 2.0f * M_PI;
	_sunDir = mlm::normalize(mlm::vec3(0.3f, sinf(skyTime), cosf(skyTime)));
	_sunPos = _sunDir * 256.0f;
}

void	Renderer::updateTime()
{
	if (!_pause)
		_time += _engine.get_delta_time() / 5.0f;
}

void	Renderer::togglePause()
{
	_pause = !_pause;
}

void	Renderer::setLightingMode(int mode)
{
	_lightingMode = mode;
}

void	Renderer::swapFrameBuffer(int direction)
{
	currentFrameBufferIdx = (currentFrameBufferIdx + frameBufferIds.size() + direction) % frameBufferIds.size();
	std::cout << "Swapped framebuffer to <" << frameBufferIds[currentFrameBufferIdx].first << ">: " << frameBufferIds[currentFrameBufferIdx].second << std::endl;
}

mlm::mat4	&Renderer::getProjection()
{
	return (_projection);
}

mlm::mat4	&Renderer::getView()
{
	return (_view);
}

mlm::mat4	&Renderer::getLightProjection()
{
	return (_lightProjection);
}

mlm::mat4	&Renderer::getLightView()
{
	return (_lightView);
}

mlm::vec3	&Renderer::getSunPos()
{
	return (_sunPos);
}
