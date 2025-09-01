/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"


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

VoxEngine::VoxEngine(): _chunkManager(*this)
{
}

VoxEngine::~VoxEngine()
{
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

	// Camera movement (maybe move into separate setup function)
	_input.addOnDownCallback(GLFW_KEY_W, [this]() {_camera.processKeyboard(Camera::FORWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_S, [this]() {_camera.processKeyboard(Camera::BACKWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_D, [this]() {_camera.processKeyboard(Camera::RIGHT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_A, [this]() {_camera.processKeyboard(Camera::LEFT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_SPACE, [this]() {_camera.processKeyboard(Camera::UP, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_LEFT_SHIFT, [this]() {_camera.processKeyboard(Camera::DOWN, get_delta_time());});

	// Random other key inputs
	_input.addOnPressCallback(GLFW_KEY_ESCAPE, std::bind(glfwSetWindowShouldClose, get_window(), GLFW_TRUE));
	_input.addOnPressCallback(GLFW_KEY_TAB, std::bind(&Input::toggleWireFrame, _input));

	glfwSetCursorPos(Window::get_window(), WINDOW_SIZE.x / 2.0f, WINDOW_SIZE.y / 2.0f);

	_camera.setPos(mlm::vec3(static_cast<float>(CHUNK_SIZE_X / 2 + 3), static_cast<float>(CHUNK_SIZE_Y / 2 + 40), static_cast<float>(CHUNK_SIZE_Z / 2 + 3)));
	_chunkManager.init();
	if (_atlas.load("./resources/textures/texture_atlas.bmp", 8) == false)
	{
		std::cerr << "Uh oh no atlas we lost :/" << std::endl;
	}
}

void	VoxEngine::mainLoop()
{

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
		
		mlm::mat4	projection = mlm::perspective(_camera.getZoom(), 0.1f, 1000.0f);
		shader.use();
		shader.set_mat4("projection", projection);

		mlm::mat4	view = _camera.getViewMatrix();
		shader.set_mat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		_atlas._texture.bind();

		_chunkManager.update();
		_chunkManager.render(shader);

		glfwSwapBuffers(Window::get_window());
		glfwPollEvents();
	}
}

void	VoxEngine::input()
{

}

void	VoxEngine::cleanup()
{
	_chunkManager.cleanup();
	_atlas.del();
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
