/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"

const float		CLIPPING_NEAR = 0.25f;
const float		CLIPPING_FAR = 640.0f;

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

void	Renderer::updateUnderWater()
{
	auto	block = _manager.getBlockType(_camera.getPos());
	_isUnderwater = (block.hasValue() && block.value() == Block::WATER);
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
