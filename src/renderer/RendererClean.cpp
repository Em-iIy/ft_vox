/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "Renderer.hpp"
#include "ShaderManager.hpp"

Renderer::~Renderer()
{}

void	Renderer::cleanup()
{
	_cleanShaders();
	_cleanFrameBuffers();
}

void	Renderer::_cleanShaders()
{
	ShaderManager::unloadShaders();
}

void	Renderer::_cleanFrameBuffers()
{
	_terrainGeometryFrameBuffer.destroy();
	_waterGeometryFrameBuffer.destroy();
	_shadowFrameBuffer.destroy();
	_terrainLightingFrameBuffer.destroy();
	_waterLightingFrameBuffer.destroy();
	_ssaoFrameBuffer.destroy();
	_ssaoBlurFrameBuffer.destroy();
	_skyFrameBuffer.destroy();
	_auroraFrameBuffer.destroy();
}
