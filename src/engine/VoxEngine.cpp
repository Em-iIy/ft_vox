/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include "ShaderManager.hpp"

#include "Frustum.hpp"

#define FPS

void	VoxEngine::run()
{
	init();
	mainLoop();
	cleanup();
}

void	VoxEngine::mainLoop()
{
	#ifdef FPS
	int frame = 0;
	float time = glfwGetTime();
	#endif
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		update();

		// Render frame and put in window
		_renderer.render();
		glfwSwapBuffers(Window::get_window());

		#ifdef FPS
		frame++;
		if (frame == 120)
		{
			std::cout << "fps: " << 1.0f / ((glfwGetTime() - time) / static_cast<float>(frame)) << std::endl;
			time = glfwGetTime();
			frame = 0;
		}
		#endif
	}
}

void	VoxEngine::update()
{
	// Poll for inputs and handle
	glfwPollEvents();
	_input.handleKeys();

	// Update deltatime among other (unused) things
	Window::update();

	_sky.update(Window::get_delta_time());
	_renderer.update();

	// Update frustums
	updateFrustum(_renderer.getProjection(), _renderer.getView());
	updateShadowFrustum(_renderer.getLightProjection(), _renderer.getLightView());

	// Update chunks (possibly redo frutsum culling !!this order is important!!)
	_chunkManager.update();
}

void	VoxEngine::cleanup()
{
	_chunkManager.cleanup();
	_renderer.cleanup();
	_atlas.del();
	glfwTerminate();
}
