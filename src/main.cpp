/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "glu/gl-utils.hpp"
#include <iostream>

int main()
{
	init_glfw();
	Window window;

	window.create_window("Hello Window", mlm::ivec2(400), Window::WINDOWED);

	while (!glfwWindowShouldClose(window.get_window()))
	{
		glfwPollEvents();
	}

	glfwTerminate();
	return (0);
}