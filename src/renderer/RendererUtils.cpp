/*
Created by: Emily (Em_iIy) Winnink
Created on: 10/03/2026
*/

#include "Renderer.hpp"

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
