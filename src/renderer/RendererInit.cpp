/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"
#include "ShaderManager.hpp"

Renderer::Renderer(VoxEngine &engine, ChunkManager &manager, Camera &camera): _engine(engine), _manager(manager), _camera(camera)
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
	ShaderManager::loadShader(_quadShader, "./resources/shaders/quad.vert", "./resources/shaders/quad.frag");

	// Render shadow map
	ShaderManager::loadShader(_shadowShader, "./resources/shaders/shadow.vert", "./resources/shaders/shadow.frag");

	// SSAO and blur shaders
	ShaderManager::loadShader(_ssaoShader, "./resources/shaders/quad.vert", "./resources/shaders/SSAO.frag", [this](){initSsaoSamples();});
	ShaderManager::loadShader(_ssaoBlurShader, "./resources/shaders/quad.vert", "./resources/shaders/SSAOBlur.frag", [this](){initSsaoBlurShader();});

	// Draw basic geometry to gBuffers
	ShaderManager::loadShader(_geometryShader, "./resources/shaders/geometry.vert", "./resources/shaders/geometry.frag");

	// Final lighting shader - Combines Light with shadows and SSAO
	ShaderManager::loadShader(_lightingShader, "./resources/shaders/quad.vert", "./resources/shaders/lighting.frag");

	// Draws water with opacity
	ShaderManager::loadShader(_waterShader, "./resources/shaders/quad.vert", "./resources/shaders/water.frag");

	// Shader for the skybox
	ShaderManager::loadShader(_skyShader, "./resources/shaders/sky.vert", "./resources/shaders/sky.frag");

	// Shader for the skybox
	ShaderManager::loadShader(_solarBodiesShader, "./resources/shaders/sky.vert", "./resources/shaders/solarBodies.frag");
	
	// Shader for Aurora
	ShaderManager::loadShader(_auroraShader, "./resources/shaders/sky.vert", "./resources/shaders/aurora.frag");

	// General UI/Debug shaders
	ShaderManager::loadShader(_cubeShader, "./resources/shaders/cube.vert", "./resources/shaders/cube.frag");
	ShaderManager::loadShader(_depthShader, "./resources/shaders/depth.vert", "./resources/shaders/depth.frag");
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
			sphereIndices.push_back(d);
			sphereIndices.push_back(c);
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
	_terrainLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	_terrainLightingFrameBuffer.unbind();
	if (_terrainLightingFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	frameBufferIds.emplace_back(_terrainLightingFrameBuffer.getColorTexture(0), "Terrain Lighting Color");
	
	_waterLightingFrameBuffer.create(size.x, size.y);
	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_waterLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
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

	_skyFrameBuffer.create(size.x, size.y);
	_skyFrameBuffer.bind();
	_skyFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_skyFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	_skyFrameBuffer.unbind();
	if (_skyFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Sky Framebuffer missing");
	frameBufferIds.emplace_back(_skyFrameBuffer.getColorTexture(0), "Sky Color");

	_auroraFrameBuffer.create(static_cast<int>(static_cast<float>(size.x) * 0.2f), static_cast<int>(static_cast<float>(size.y) * 0.2f));
	_auroraFrameBuffer.bind();
	_auroraFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, false, true, false);
	_auroraFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	_auroraFrameBuffer.unbind();
	if (_auroraFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Aurora Framebuffer missing");
	frameBufferIds.emplace_back(_auroraFrameBuffer.getColorTexture(0), "Aurora Color");
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
}

void	Renderer::initSsaoBlurShader()
{
	_ssaoBlurShader.use();
	_ssaoBlurShader.set_int("uTexture", 0);
}

void	Renderer::initSsaoNoise()
{
	std::array<mlm::vec3, 8>	ssaoNoise;

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
