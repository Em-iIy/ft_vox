/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"

#include <unordered_map>

mlm::vec3	randVec3()
{
	static rng::fgen	gen = rng::generator(-1.0f, 1.0f);

	return (
		mlm::vec3(
			rng::rand(gen),
			rng::rand(gen),
			rng::rand(gen)
		)
	);
}

void	VoxEngine::run()
{
	init();
	mainLoop();
	cleanup();
}

void	VoxEngine::init()
{
	rng::seed();
	init_glfw();
	Window::create_window("ft_vox", WINDOW_SIZE, Window::WINDOWED);
	glfwSetWindowUserPointer(Window::get_window(), this);
	_input.init(Window::get_window(), Window::get_size());

}

void	VoxEngine::mainLoop()
{

	std::vector<Vertex> vertices = {
		{mlm::vec3(-0.5f, -0.5f, -0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, // 0 back bottom left
		{mlm::vec3(0.5f, -0.5f, -0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  1 back bottom right
		{mlm::vec3(-0.5f, 0.5f, -0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  2 back top left
		{mlm::vec3(0.5f, 0.5f, -0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   3 back top right
		{mlm::vec3(-0.5f, -0.5f, 0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //  4 front bottom left
		{mlm::vec3(0.5f, -0.5f, 0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   5 front bottom right
		{mlm::vec3(-0.5f, 0.5f, 0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}, //   6 front top left
		{mlm::vec3(0.5f, 0.5f, 0.5f), mlm::vec3(0.0f), mlm::vec2(0.0f)}  //    7 front top right
	};

	std::vector<uint32_t> indices = {
		// back face
		0, 2, 1,
		1, 3, 2,
		// front face
		4, 6, 5,
		5, 7, 6,
		// left face
		0, 4, 6,
		0, 2, 6,
		// right face
		1, 5, 7,
		1, 7, 3,
		// top face
		2, 6, 7,
		2, 7, 3,
		// bottom face
		1, 0, 4,
		1, 5, 4
	};


	_camera.setPos(mlm::vec3(static_cast<float>(CHUNK_SIZE_X / 2 + 3), static_cast<float>(CHUNK_SIZE_Y / 2 + 3), static_cast<float>(CHUNK_SIZE_Z / 2 + 3)));
	glfwSetCursorPos(Window::get_window(), WINDOW_SIZE.x / 2.0f, WINDOW_SIZE.y / 2.0f);

	Mesh	cube(vertices, indices);

	[[maybe_unused]] std::unordered_map<mlm::ivec2, Chunk, ivec2Hash> chunkMap;

	for (int x = -5; x <= 5; x++)
	{
		for (int y = -5; y <= 5; y++)
		{
			mlm::ivec2	pos(x, y);
			chunkMap[pos] = Chunk(pos);
			chunkMap[pos].generate();
		}
	}


	Shader	shader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_input.handleKeys();
		Window::update();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mlm::ivec2	size;
		
		mlm::mat4	projection = mlm::perspective(_camera.getZoom(), 0.1f, 200.0f);
		shader.use();
		shader.set_mat4("projection", projection);

		mlm::mat4	view = _camera.getViewMatrix();
		shader.set_mat4("view", view);

		mlm::mat4	model(1.0f);
		shader.set_mat4("model", model);
		shader.set_vec3("color", mlm::vec3(0.3f, 0.2f, 0.3f));
		for (auto &[_, chunk]: chunkMap)
		{
			chunk.draw(shader);
		}

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
