/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include "ShaderManager.hpp"
#include "Frustum.hpp"
#include "Logger.hpp"

void	VoxEngine::run()
{
	_init();
	_mainLoop();
	_cleanup();
}

void	VoxEngine::_mainLoop()
{
	int frame = 0;
	float time = glfwGetTime();
	Logger::info("Engine: Starting main loop!");
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_update();

		// Render frame and put in window
		_renderer.render();
		glfwSwapBuffers(Window::get_window());

		frame++;
		if (frame == 120)
		{
			float fps = 1.0f / ((glfwGetTime() - time) / static_cast<float>(frame));
			Logger::log("FPS: " + std::to_string(fps));
			time = glfwGetTime();
			frame = 0;
		}
	}
}

void	VoxEngine::_update()
{
	// Poll for inputs and handle
	glfwPollEvents();
	_input.handleKeys();

	// Update deltatime among other (unused) things
	Window::update();

	_sky.update(Window::get_delta_time());
	_renderer.update();

	// Update frustums
	_updateFrustum(_renderer.getProjection(), _renderer.getView());
	_updateShadowFrustum(_renderer.getLightProjection(), _renderer.getLightView());

	// Update chunks (possibly redo frutsum culling !!this order is important!!)
	_chunkManager.update();
}

void	VoxEngine::_cleanup()
{
	_chunkManager.cleanup();
	_renderer.cleanup();
	_atlas.del();
	glfwTerminate();
}
