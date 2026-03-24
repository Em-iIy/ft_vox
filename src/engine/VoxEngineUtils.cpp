/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#include "VoxEngine.hpp"

void	VoxEngine::_updateFrustum(const mlm::mat4 &projection, const mlm::mat4 &view)
{
	if (!_frustumUpdate)
		return ;
	_frustum.update(projection * view);
	_frustumUpdate = false;
}

void	VoxEngine::_updateShadowFrustum(const mlm::mat4 &projection, const mlm::mat4 &view)
{
	_shadowFrustum.update(projection * view);
}

Camera	&VoxEngine::getCamera()
{
	return (_camera);
}

Input	&VoxEngine::getInput()
{
	return (_input);
}

ChunkManager	&VoxEngine::getManager()
{
	return (_chunkManager);
}

Atlas	&VoxEngine::getAtlas()
{
	return (_atlas);
}

Frustum	&VoxEngine::getFrustum()
{
	return (_frustum);
}

Frustum	&VoxEngine::getShadowFrustum()
{
	return (_shadowFrustum);
}

Sky	&VoxEngine::getSky()
{
	return (_sky);
}

void	VoxEngine::setFrustumUpdate()
{
	_frustumUpdate = true;
}
