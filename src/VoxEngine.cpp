/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"

#include "../lib/glu/lib/emlm/mlm/print/vectorPrint.hpp"

void	VoxEngine::run()
{
	init();
	mainLoop();
	cleanup();
}

void	VoxEngine::init()
{
	init_glfw();
	Window::create_window("ft_vox", WINDOW_SIZE, Window::WINDOWED);
	glfwSetWindowUserPointer(Window::get_window(), this);
	_input.init(Window::get_window(), Window::get_size());

}

void	VoxEngine::mainLoop()
{

	std::vector<Vertex>	vertices = {
		{mlm::vec3(-1.0f, -1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, // 0 back bottom left
		{mlm::vec3(1.0f, -1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  1 back bottom right
		{mlm::vec3(-1.0f, 1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  2 back top left
		{mlm::vec3(1.0f, 1.0f, -1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   3 back top right
		{mlm::vec3(-1.0f, -1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  4 front bottom left
		{mlm::vec3(1.0f, -1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   5 front bottom right
		{mlm::vec3(-1.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   6 front top left
		{mlm::vec3(1.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)} //     7 front top right
	};

	std::vector<uint32_t> indices = {
		// back face
		0, 1, 2,
		1, 2, 3,
		// front face
		4, 5, 6,
		5, 6, 7,
		// left face
		0, 6, 4,
		0, 6, 2,
		// right face
		1, 7, 5,
		1, 7, 3,
		// top face
		2, 7, 6,
		2, 7, 3,
		// bottom face
		1, 4, 0,
		1, 4, 5,	
	};

	Mesh	cube(vertices, indices);

	Shader	shader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");

	glEnable(GL_DEPTH);
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_input.handleKeys();
		Window::update();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mlm::ivec2	size;
		
		mlm::mat4	projection = mlm::perspective(_camera.getZoom(), 0.001f, 100.0f);
		float time = glfwGetTime();
		time = 1.f;
		shader.use();
		shader.set_mat4("projection", projection);
		shader.set_vec3("color", mlm::vec3(1.0f, 0.0f, 1.0f));

		mlm::mat4	model(1.0f);
		model = mlm::translate(model, mlm::vec3(0.0f, 0.0f, -50.f));
		model = mlm::rotate(model, time, mlm::vec3(1.0f, 0.0f, 0.0f));
		model = mlm::rotate(model, time, mlm::vec3(0.0f, 0.0f, 1.0f));
		// model = mlm::scale(model, mlm::vec3(1.8f));
		shader.set_mat4("model", model);

		mlm::mat4	view = _camera.getViewMatrix();
		shader.set_mat4("view", view);

		cube.draw(shader);
		// glfwGetFramebufferSize(Window::get_window(), &size.x, &size.y);
		// std::cout << "current size " << size << std::endl;
		glfwSwapBuffers(Window::get_window());
		glfwPollEvents();
	}
}

void	VoxEngine::input()
{

}

void	VoxEngine::cleanup()
{
	glfwTerminate();
}

Camera	&VoxEngine::getCamera()
{
	return (_camera);
}

Input	&VoxEngine::getInput()
{
	return (_input);
}
