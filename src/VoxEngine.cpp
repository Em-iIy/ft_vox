/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"

void	VoxEngine::run()
{
	init();
	mainLoop();
	cleanup();
}

void	VoxEngine::init()
{
	init_glfw();
	_window.create_window("ft_vox", WINDOW_SIZE, Window::WINDOWED);
}

void	VoxEngine::mainLoop()
{
	while (!glfwWindowShouldClose(_window.get_window()))
	{
		glfwPollEvents();
	}
}

void	VoxEngine::cleanup()
{
	glfwTerminate();
}