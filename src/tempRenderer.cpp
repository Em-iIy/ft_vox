/*
Created by: Emily (Em_iIy) Winnink
Created on: 29/09/2025
*/

#include "Renderer.hpp"
#include "VoxEngine.hpp"



void			Renderer::temp_render()
{
	FrameBuffer::unbind();
	FrameBuffer::clear(true, true, mlm::vec4(_bgColor, 1.0f));

	temp_shadowPass();
	temp_terrainGeometryPass();
	temp_waterGeometryPass();
	temp_SSAOPass();
	temp_terrainLightingPass();
	temp_waterLightingPass();

	temp_renderFinal();
}

void			Renderer::temp_initShaders()
{
	_quadShader = Shader("./resources/temp-shaders/quad.vert", "./resources/temp-shaders/quad.frag");
	_shadowShader = Shader("./resources/temp-shaders/shadow.vert", "./resources/temp-shaders/shadow.frag");
	_ssaoShader = Shader("./resources/temp-shaders/quad.vert", "./resources/temp-shaders/SSAO.frag");
	_ssaoBlurShader = Shader("./resources/temp-shaders/quad.vert", "./resources/temp-shaders/SSAOBlur.frag");
	_waterShader = Shader("./resources/temp-shaders/water.vert", "./resources/temp-shaders/water.frag");
	temp_geometryShader = Shader("./resources/temp-shaders/geometry.vert", "./resources/temp-shaders/geometry.frag");
	temp_lightingShader = Shader("./resources/temp-shaders/quad.vert", "./resources/temp-shaders/lighting.frag");
}

void			Renderer::temp_initFrameBuffers()
{
	temp_FrameBufferIds.emplace_back(0, "Default");

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
	temp_FrameBufferIds.emplace_back(_geometryFrameBuffer.getColorTexture(0), "Geometry Color");
	temp_FrameBufferIds.emplace_back(_geometryFrameBuffer.getColorTexture(1), "Geometry Normal");
	temp_FrameBufferIds.emplace_back(_geometryFrameBuffer.getColorTexture(2), "Geometry Position");

	_waterFrameBuffer.create(size.x, size.y);
	_waterFrameBuffer.bind();
	_waterFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_waterFrameBuffer.attachColorTexture(1, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_waterFrameBuffer.attachColorTexture(2, GL_RGB16F, GL_RGBA, GL_FLOAT, true, true, false);
	_waterFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_waterFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_waterFrameBuffer.unbind();
	if (_waterFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	temp_FrameBufferIds.emplace_back(_waterFrameBuffer.getColorTexture(0), "Water Color");
	temp_FrameBufferIds.emplace_back(_waterFrameBuffer.getColorTexture(1), "Water Normal");
	temp_FrameBufferIds.emplace_back(_waterFrameBuffer.getColorTexture(2), "Water Position");

	_terrainLightingFrameBuffer.create(size.x, size.y);
	_terrainLightingFrameBuffer.bind();
	_terrainLightingFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_terrainLightingFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_terrainLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_terrainLightingFrameBuffer.unbind();
	if (_terrainLightingFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	temp_FrameBufferIds.insert(temp_FrameBufferIds.begin(), {_terrainLightingFrameBuffer.getColorTexture(0), "Terrain Lighting Color"});
	// temp_FrameBufferIds.emplace_back(_terrainLightingFrameBuffer.getColorTexture(0), "Terrain Lighting Color");
	
	_waterLightingFrameBuffer.create(size.x, size.y);
	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.attachColorTexture(0, GL_RGBA8, GL_RGBA, GL_FLOAT, true, true, false);
	_waterLightingFrameBuffer.ensureDepthRbo(GL_DEPTH24_STENCIL8);
	_waterLightingFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	_waterLightingFrameBuffer.unbind();
	if (_waterLightingFrameBuffer.checkStatus() == false)
		throw std::runtime_error("Water Framebuffer missing");
	temp_FrameBufferIds.insert(temp_FrameBufferIds.begin(), {_waterLightingFrameBuffer.getColorTexture(0), "Water Lighting Color"});
	// temp_FrameBufferIds.emplace_back(_waterLightingFrameBuffer.getColorTexture(0), "Water Lighting Color");

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
	temp_FrameBufferIds.emplace_back(_ssaoFrameBuffer.getColorTexture(0), "SSAO Color");

	_ssaoBlurFrameBuffer.create(size.x, size.y);
	_ssaoBlurFrameBuffer.bind();
	_ssaoBlurFrameBuffer.attachColorTexture(0, GL_RED, GL_RED, GL_FLOAT, true, true, false);
	_ssaoBlurFrameBuffer.setDrawBuffers({GL_COLOR_ATTACHMENT0});
	if (_ssaoBlurFrameBuffer.checkStatus() == false)
		throw std::runtime_error("SSAO Blur Framebuffer missing");
	temp_FrameBufferIds.emplace_back(_ssaoBlurFrameBuffer.getColorTexture(0), "SSAO Blur Color");
}

void			Renderer::temp_swapFrameBuffer(int direction)
{
	temp_currentFrameBufferIdx = (temp_currentFrameBufferIdx + temp_FrameBufferIds.size() + direction) % temp_FrameBufferIds.size();
	std::cout << "Swapped framebuffer to <" << temp_FrameBufferIds[temp_currentFrameBufferIdx].first << ">: " << temp_FrameBufferIds[temp_currentFrameBufferIdx].second << std::endl;
}

void			Renderer::temp_shadowPass()
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

void			Renderer::temp_terrainGeometryPass()
{
	temp_geometryShader.use();

	temp_geometryShader.set_mat4("uProjection", _projection);
	temp_geometryShader.set_mat4("uView", _view);
	
	glActiveTexture(GL_TEXTURE0);
	_engine.getAtlas().bind();
	temp_geometryShader.set_int("uAtlas", 0);

	_geometryFrameBuffer.bind();
	FrameBuffer::clearBufferfv(GL_COLOR, 0, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 1, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 2, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clear(false, true, mlm::vec4(0.0f));

	_manager.renderChunks(temp_geometryShader);
}

void			Renderer::temp_waterGeometryPass()
{
	temp_geometryShader.use();

	temp_geometryShader.set_mat4("uProjection", _projection);
	temp_geometryShader.set_mat4("uView", _view);
	
	glActiveTexture(GL_TEXTURE0);
	_engine.getAtlas().bind();
	temp_geometryShader.set_int("uAtlas", 0);

	_waterFrameBuffer.bind();
	FrameBuffer::clearBufferfv(GL_COLOR, 0, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 1, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clearBufferfv(GL_COLOR, 2, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	FrameBuffer::clear(false, true, mlm::vec4(0.0f));

	mlm::ivec2	size = _engine.get_size();
	_waterFrameBuffer.blitDepthFrom(_geometryFrameBuffer.getId(), size.x, size.y);
	_waterFrameBuffer.bind();

	_manager.renderWater(temp_geometryShader);
}

void			Renderer::temp_SSAOPass()
{
	_ssaoShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(2));
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

void			Renderer::temp_terrainLightingPass()
{
	temp_lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(0));
	temp_lightingShader.set_int("uGColor", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(1));
	temp_lightingShader.set_int("uGNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _geometryFrameBuffer.getColorTexture(2));
	temp_lightingShader.set_int("uGPosition", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());
	temp_lightingShader.set_int("uShadowMap", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurFrameBuffer.getColorTexture(0));
	temp_lightingShader.set_int("uSSAO", 4);

	// temp_lightingShader.set_vec3("uLightPos", _sunDir)
	temp_lightingShader.set_vec3("uLightDir", _sunDir);
	temp_lightingShader.set_mat4("uLightView", _lightView);
	temp_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	temp_lightingShader.set_mat4("uView", _view);

	temp_lightingShader.set_bool("uIsWater", false);

	_terrainLightingFrameBuffer.bind();
	_terrainLightingFrameBuffer.clear(true, true, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	_quadMesh.draw(temp_lightingShader);
}

void			Renderer::temp_waterLightingPass()
{
	temp_lightingShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer.getColorTexture(0));
	temp_lightingShader.set_int("uGColor", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer.getColorTexture(1));
	temp_lightingShader.set_int("uGNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _waterFrameBuffer.getColorTexture(2));
	temp_lightingShader.set_int("uGPosition", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _shadowFrameBuffer.getDepthTexture());
	temp_lightingShader.set_int("uShadowMap", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _ssaoBlurFrameBuffer.getColorTexture(0));
	temp_lightingShader.set_int("uSSAO", 4);

	// temp_lightingShader.set_vec3("uLightPos", _sunDir)
	temp_lightingShader.set_vec3("uLightDir", _sunDir);
	temp_lightingShader.set_mat4("uLightView", _lightView);
	temp_lightingShader.set_mat4("uLightProjection", _lightProjection);
	
	temp_lightingShader.set_mat4("uView", _view);

	temp_lightingShader.set_bool("uIsWater", true);

	_waterLightingFrameBuffer.bind();
	_waterLightingFrameBuffer.clear(true, true, mlm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	_quadMesh.draw(temp_lightingShader);
}

void			Renderer::temp_renderFinal()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	_quadShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _terrainLightingFrameBuffer.getColorTexture(0));
	_quadShader.set_int("uRenderTex", 0);
	_quadMesh.draw(_quadShader);

	glClear(GL_DEPTH_BUFFER_BIT);

	_waterShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterLightingFrameBuffer.getColorTexture(0));
	_waterShader.set_int("uRenderTex", 0);
	_waterShader.set_float("uWaterOpacity", 0.9f);
	_quadMesh.draw(_waterShader);
	// _quadShader.use();
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, temp_FrameBufferIds[temp_currentFrameBufferIdx].first);
	// _quadShader.set_int("uRenderTex", 0);

	// _quadMesh.draw(_quadShader);
}
