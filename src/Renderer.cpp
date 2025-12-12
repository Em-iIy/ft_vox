/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"

const mlm::vec3	SKY_COLOR_DAY(0.4f, 0.7f, 0.9f);
const mlm::vec3	SKY_COLOR_NIGHT(0.05f, 0.1f, 0.2f);
const mlm::vec3	WATER_COLOR(0.0f, 0.0f, 0.8f);

const float		FOG_WATER_NEAR = 0.0f;
const float		FOG_NEAR = 120.0f;
const float		FOG_FAR = 160.0f;

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
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

}

void	Renderer::initShaders()
{
	_chunkShader = Shader("./resources/shaders/chunk.vert", "./resources/shaders/chunk.frag");
	_cubeShader = Shader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");
	_quadShader = Shader("./resources/shaders/quad.vert", "./resources/shaders/quad.frag");
	_depthShader = Shader("./resources/shaders/depth.vert", "./resources/shaders/depth.frag");
	_waterShader = Shader("./resources/shaders/water.vert", "./resources/shaders/water.frag");
	_shadowShader = Shader("./resources/shaders/shadow.vert", "./resources/shaders/shadow.frag");
	_ssaoShader = Shader("./resources/shaders/chunk.vert", "./resources/shaders/SSAO.frag");
}

void	Renderer::initMeshes()
{
	std::vector<Vertex>		cubeVertices = {
		{mlm::vec3(0.0f, 0.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 0.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 0.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 0.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
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
}

void	Renderer::initFrameBuffers()
{
	mlm::ivec2	size = _engine.get_size();
	_geometryFrameBuffer.create(size.x, size.y);
	_geometryFrameBuffer.bind();
	_geometryFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_geometryFrameBuffer.attachColorTexture(1, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_geometryFrameBuffer.attachColorTexture(2, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_geometryFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_geometryFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_geometryFrameBuffer.unbind();
	if (_geometryFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");

	_waterFrameBuffer.create(size.x, size.y);
	_waterFrameBuffer.bind();
	_waterFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true, true, false);
	_waterFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_waterFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	_waterFrameBuffer.unbind();
	if (_waterFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	
	_shadowFrameBuffer.create(4096, 4096);
	_shadowFrameBuffer.bind();
	_shadowFrameBuffer.ensureDepthTexture(GL_DEPTH_COMPONENT, GL_FLOAT, true, GL_CLAMP_TO_BORDER, mlm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_shadowFrameBuffer.setDrawBuffers({});
	_shadowFrameBuffer.unbind();
	if (_shadowFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Shadow Framebuffer missing");
}

void	Renderer::initSsaoSamples()
{
	std::array<mlm::vec3, 64>		ssaoSamples;

	static rng::fgen	gen = rng::generator(0.0f, 1.0f);

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
}

void	Renderer::cleanup()
{
	cleanShaders();
	cleanMeshes();
	cleanFrameBuffers();
}

void	Renderer::cleanShaders()
{
	_chunkShader.del();
	_cubeShader.del();
	_quadShader.del();
	_depthShader.del();
	_waterShader.del();
	_shadowShader.del();
	_ssaoShader.del();
}

void	Renderer::cleanMeshes()
{
	// _cubeMesh.del()
	// _quadMesh.del()
}

void	Renderer::cleanFrameBuffers()
{
	_geometryFrameBuffer.destroy();
	_waterFrameBuffer.destroy();
	_shadowFrameBuffer.destroy();
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
	FrameBuffer::clear(true, true, mlm::vec4(_bgColor, 1.0f));

	renderSun();
	renderChunks();
	renderUI();
}

void	Renderer::renderChunks()
{
	_chunkShader.use();
	updateChunkShader();
	renderShadowMap();
	renderTerrain();
	renderWater();
	renderFinal();
	renderWaterFinal();
}

void	Renderer::updateChunkShader()
{
	_chunkShader.set_mat4("uProjection", _projection);
	_chunkShader.set_mat4("uView", _view);

	if (_isUnderwater)
		_chunkShader.set_float("uFogNear", FOG_WATER_NEAR);
	else
		_chunkShader.set_float("uFogNear", FOG_NEAR);

	_chunkShader.set_float("uFogFar", FOG_FAR);
	_chunkShader.set_vec3("uFogColor", _bgColor);
	_chunkShader.set_vec3("uLightDir", _sunDir);

	_chunkShader.set_int("uLightingMode", _lightingMode);

	_engine.getAtlas().bind();
}

void	Renderer::renderShadowMap()
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

void	Renderer::renderTerrain()
{
	// return ;
	_chunkShader.use();

	_chunkShader.set_mat4("uLightProjection", _lightProjection);
	_chunkShader.set_mat4("uLightView", _lightView);
	glActiveTexture(GL_TEXTURE0);
	_engine.getAtlas().bind();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());

	_chunkShader.set_int("uAtlas", 0);
	_chunkShader.set_int("uShadowMap", 1);

	_geometryFrameBuffer.bind();
	FrameBuffer::clearBufferfv(GL_COLOR, 0,  mlm::vec4(_bgColor, 1.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 1, mlm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 2, mlm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	FrameBuffer::clear(false, true, mlm::vec4(0.0f));

	_manager.renderChunks(_chunkShader);
}

void	Renderer::renderWater()
{
	// return ;
	_waterFrameBuffer.bind();
	_waterFrameBuffer.clear(true, true, mlm::vec4(0.0f));

	mlm::ivec2	size = _engine.get_size();
	_waterFrameBuffer.blitDepthFrom(_geometryFrameBuffer.getId(), size.x, size.y);
	_waterFrameBuffer.bind();
	

	glDisable(GL_CULL_FACE);
	_manager.renderWater(_chunkShader);
	glEnable(GL_CULL_FACE);
	_waterFrameBuffer.unbind();

	_manager.renderClear();
}

void	Renderer::renderWaterFinal()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	FrameBuffer::clear(false, true, mlm::vec4(1.0));


	bool wireFrameMode = _engine.getInput().getWireFrameMode();
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_waterShader.use();
	_waterShader.set_float("uWaterOpacity", 0.7f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer.getColorTexture(0));
	_waterShader.set_int("uRenderTex", 0);
	_quadMesh.draw(_waterShader);

	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void	Renderer::renderSun()
{
	mlm::mat4	model;

	_cubeShader.use();
	_cubeShader.set_mat4("uProjection", _projection);
	_cubeShader.set_mat4("uView", _view);

	// draw sun
	model = mlm::translate(mlm::mat4(1.0f), _sunPos * 2.0f);
	model = mlm::scale(model, mlm::vec3(5.0f));
	_cubeShader.set_mat4("uModel", model);

	_cubeShader.set_vec3("uColor", mlm::vec3(1.f, 1.0f, .5f));
	_cubeShader.set_float("uAlpha", 1.0f);

	_cubeMesh.draw(_cubeShader);

	// draw moon
	model = mlm::translate(mlm::mat4(1.0f), -1.0f * _sunPos);
	model = mlm::scale(model, mlm::vec3(2.0f));
	_cubeShader.set_mat4("uModel", model);

	_cubeShader.set_vec3("uColor", mlm::vec3(1.f, 1.0f, 1.0f));
	_cubeShader.set_float("uAlpha", 1.0f);

	_cubeMesh.draw(_cubeShader);
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

void	Renderer::renderFinal()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bool wireFrameMode = _engine.getInput().getWireFrameMode();
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_quadShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(0));
	_quadShader.set_int("uRenderTex", 0);
	_quadMesh.draw(_quadShader);

	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void			Renderer::updateProjection()
{
	mlm::vec2	size = static_cast<mlm::vec2>(_engine.get_size());
	_projection = mlm::perspective(_camera.getZoom(), size.x / size.y, CLIPPING_NEAR, CLIPPING_FAR);
}

void			Renderer::updateView()
{
	_view = _camera.getViewMatrix();
}

void			Renderer::updateLightProjection()
{
	_lightProjection = mlm::ortho(-160.0f, 160.0f, -160.0f, 160.0f, 0.1f, 512.0f);
}

void			Renderer::updateLightView()
{
	_lightView = mlm::lookat(_sunPos, mlm::vec3(0.0f), mlm::vec3(0.0f, 1.0f, 0.0f));
}

mlm::vec3	v3Lerp(const mlm::vec3 &v1, const mlm::vec3 &v2, float t)
{
	return (v1 + t * (v2 - v1));
}

void			Renderer::updateUnderWater()
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

void			Renderer::updateSunPos()
{
	_sunDir = mlm::normalize(mlm::vec3(0.3f, sinf(_time), cosf(_time)));
	_sunPos = _sunDir * 256.0f;
}

void			Renderer::updateTime()
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