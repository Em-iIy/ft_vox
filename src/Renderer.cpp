/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"

const mlm::vec3	SKY_COLOR(0.4f, 0.7f, 0.9f);
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
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

}

void	Renderer::initShaders()
{
	_chunkShader = Shader("./resources/shaders/chunk.vert", "./resources/shaders/chunk.frag");
	_cubeShader = Shader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");
	_waterShader = Shader("./resources/shaders/water.vert", "./resources/shaders/water.frag");
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
	_waterFrameBuffer.create(size.x, size.y);
	_waterFrameBuffer.bind();
	_waterFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true, true, false);
	_waterFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_waterFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	_waterFrameBuffer.unbind();
	if (_waterFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
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
	_waterShader.del();
	_quadShader.del();
}

void	Renderer::cleanMeshes()
{
	// _cubeMesh.del()
	// _quadMesh.del()
}

void	Renderer::cleanFrameBuffers()
{
	_waterFrameBuffer.destroy();

}

void	Renderer::update()
{
	updateProjection();
	updateView();
	updateUnderWater();
}

void	Renderer::render()
{
	FrameBuffer::clear(true, true, mlm::vec4(_bgColor, 1.0f));
	glActiveTexture(GL_TEXTURE0);

	renderChunks();
	renderUI();
}

void	Renderer::renderChunks()
{
	_chunkShader.use();
	updateChunkShader();
	renderTerrain();
	renderWater();
}

void	Renderer::updateChunkShader()
{
	_chunkShader.set_mat4("projection", _projection);
	_chunkShader.set_mat4("view", _view);

	if (_isUnderwater)
		_chunkShader.set_float("uFogNear", FOG_WATER_NEAR);
	else
		_chunkShader.set_float("uFogNear", FOG_NEAR);

	_chunkShader.set_float("uFogFar", FOG_FAR);
	_chunkShader.set_vec3("uFogColor", _bgColor);

	_engine.getAtlas().bind();
}

void	Renderer::renderTerrain()
{
	_manager.renderChunks(_chunkShader);
}

void	Renderer::renderWater()
{
	_waterFrameBuffer.bind();
	_waterFrameBuffer.clear(true, true, mlm::vec4(0.0f));

	mlm::ivec2	size = _engine.get_size();
	_waterFrameBuffer.blitDepthFrom(0, size.x, size.y);
	_waterFrameBuffer.bind();
	
	_manager.renderWater(_chunkShader);
	_waterFrameBuffer.unbind();

	_manager.renderClear();

	bool wireFrameMode = _engine.getInput().getWireFrameMode();
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_waterShader.use();
	_waterShader.set_float("uWaterOpacity", 0.7f);
	glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer.getColorTexture(0));
	_quadMesh.draw(_waterShader);

	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void	Renderer::renderUI()
{
	Expected<mlm::ivec3, bool>	rayWorldCoord = _manager.castRayIncluding();
	if (rayWorldCoord.hasValue())
	{
		_cubeShader.use();
		_cubeShader.set_mat4("projection", _projection);
		_cubeShader.set_mat4("view", _view);
		mlm::mat4	model(1.0f);
		mlm::vec3	pos = static_cast<mlm::vec3>(rayWorldCoord.value()) - _camera.getPos();
		model = mlm::translate(model, pos);
		_cubeShader.set_mat4("model", model);
		_cubeMesh.draw(_cubeShader);
	}
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
		_bgColor = SKY_COLOR;
		_isUnderwater = false;
	}
}

mlm::mat4	&Renderer::getProjection()
{
	return (_projection);
}

mlm::mat4	&Renderer::getView()
{
	return (_view);
}

