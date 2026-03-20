/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"
#include "ShaderManager.hpp"

void	Renderer::update()
{
	// updateTime();
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
	renderSkyColor();
	terrainLightingPass();
	waterLightingPass();
	renderSky();
	renderFinal();
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

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _skyFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uSky", 5);

	_lightingShader.set_vec3("uLightDir", _sunDir);
	_lightingShader.set_mat4("uLightView", _lightView);
	_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	_lightingShader.set_mat4("uView", _view);

	Sky &sky = _engine.getSky();
	sky.setFog(_lightingShader, _isUnderwater);

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

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _skyFrameBuffer.getColorTexture(0));
	_lightingShader.set_int("uSky", 5);

	_lightingShader.set_vec3("uLightDir", _sunDir);
	_lightingShader.set_mat4("uLightView", _lightView);
	_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	_lightingShader.set_mat4("uView", _view);

	Sky &sky = _engine.getSky();
	sky.setFog(_lightingShader, _isUnderwater);

	_lightingShader.set_bool("uIsWater", true);

	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.clear(true, true, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	glDisable(GL_DEPTH_TEST);
	_quadMesh.draw(_lightingShader);
	glEnable(GL_DEPTH_TEST);
}

void	Renderer::renderSky()
{
	_skyFrameBuffer.bind();

	glDisable(GL_DEPTH_TEST);
	renderSolarBodies();
	renderAurora();
	glEnable(GL_DEPTH_TEST);
}

void	Renderer::renderSkyColor()
{
	_skyFrameBuffer.bind();

	_skyShader.use();
	_skyShader.set_mat4("uProjection", _projection);
	_skyShader.set_mat4("uView", _view);

	Sky &sky = _engine.getSky();
	_skyShader.set_vec3("uSunDir", _sunDir);
	_skyShader.set_float("uTime", glfwGetTime());

	sky.setGradient(_skyShader);

	_sphereMesh.draw(_skyShader);
}

void	Renderer::renderSolarBodies()
{
	_skyFrameBuffer.bind();

	_solarBodiesShader.use();
	_solarBodiesShader.set_mat4("uProjection", _projection);
	_solarBodiesShader.set_mat4("uView", _view);

	Sky &sky = _engine.getSky();
	_solarBodiesShader.set_vec3("uSunDir", _sunDir);
	sky.setSolarBodies(_solarBodiesShader);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	_sphereMesh.draw(_solarBodiesShader);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void	Renderer::renderAurora()
{
	// Draw aurora to scaled down framebuffer first
	_auroraFrameBuffer.bind();
	_auroraFrameBuffer.clear(true, false, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	glViewport(0, 0, _auroraFrameBuffer.getWidth(), _auroraFrameBuffer.getHeight());

	_auroraShader.use();
	_auroraShader.set_mat4("uProjection", _projection);
	_auroraShader.set_mat4("uView", _view);

	_auroraShader.set_vec3("uSunDir", _sunDir);
	_auroraShader.set_float("uTime", glfwGetTime());
	Sky &sky = _engine.getSky();
	float	tempNightFactor = sinf(sky.getNightTimePercent() * M_PI);
	_auroraShader.set_float("uNightFactor", tempNightFactor);

	_sphereMesh.draw(_auroraShader);

	// Draw aurora texture scaled up to the sky framebuffer
	_skyFrameBuffer.bind();
	glViewport(0, 0, _skyFrameBuffer.getWidth(), _skyFrameBuffer.getHeight());

	_quadShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _auroraFrameBuffer.getColorTexture(0));
	_quadShader.set_int("uRenderTex", 0);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	_quadMesh.draw(_quadShader);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void	Renderer::renderFinal()
{
	FrameBuffer::unbind();
	glDisable(GL_DEPTH_TEST);

	_quadShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _skyFrameBuffer.getColorTexture(0));
	_quadShader.set_int("uRenderTex", 0);
	_quadMesh.draw(_quadShader);

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
}

