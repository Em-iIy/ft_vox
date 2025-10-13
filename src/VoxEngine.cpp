/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"

#include "Frustum.hpp"


// #define FPS

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

mlm::ivec3	getWorldCoord(const mlm::vec3 &coord);

VoxEngine::VoxEngine(): _chunkManager(*this), _renderer(*this, _chunkManager, _camera)
{}

VoxEngine::~VoxEngine()
{}

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
	// Window::create_window("ft_vox", WINDOW_SIZE, Window::FULL_SCREEN_WINDOWED);
	Window::create_window("ft_vox", WINDOW_SIZE, Window::WINDOWED);
	glfwSwapInterval(0);
	glfwSetWindowUserPointer(Window::get_window(), this);
	_input.init(Window::get_window(), Window::get_size());

	// Reload all chunks?
	_input.addOnPressCallback(GLFW_KEY_R, [this]() {_chunkManager.unloadAll();});

	// Camera movement (maybe move into separate setup function)
	_input.addOnDownCallback(GLFW_KEY_W, [this]() {_camera.processKeyboard(Camera::FORWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_S, [this]() {_camera.processKeyboard(Camera::BACKWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_D, [this]() {_camera.processKeyboard(Camera::RIGHT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_A, [this]() {_camera.processKeyboard(Camera::LEFT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_SPACE, [this]() {_camera.processKeyboard(Camera::UP, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_LEFT_SHIFT, [this]() {_camera.processKeyboard(Camera::DOWN, get_delta_time());});

	// Sprinting
	_input.addOnPressCallback(GLFW_KEY_LEFT_CONTROL, [this]() {_camera.toggleSprint();});
	_input.addOnReleaseCallback(GLFW_KEY_LEFT_CONTROL, [this]() {_camera.toggleSprint();});

	_input.addOnPressCallback(GLFW_KEY_1, [this]() {_player.setActiveBlock(Block::DIRT);});
	_input.addOnPressCallback(GLFW_KEY_2, [this]() {_player.setActiveBlock(Block::GRASS);});
	_input.addOnPressCallback(GLFW_KEY_3, [this]() {_player.setActiveBlock(Block::STONE);});
	_input.addOnPressCallback(GLFW_KEY_4, [this]() {_player.setActiveBlock(Block::WATER);});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_LEFT, [this]() {_chunkManager.placeBlock(_player.getActiveBlock());});
	_input.addOnDownCallback(GLFW_MOUSE_BUTTON_MIDDLE, [this]() {_chunkManager.setBlock(_camera.getPos(), _player.getActiveBlock());});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_RIGHT, [this]() {_chunkManager.deleteBlock();});

	// Random other key inputs
	_input.addOnPressCallback(GLFW_KEY_ESCAPE, std::bind(glfwSetWindowShouldClose, get_window(), GLFW_TRUE));
	_input.addOnPressCallback(GLFW_KEY_TAB, [this]() {_input.toggleWireFrame();});

	glfwSetCursorPos(Window::get_window(), WINDOW_SIZE.x / 2.0f, WINDOW_SIZE.y / 2.0f);

	_camera.setPos(mlm::vec3(static_cast<float>(CHUNK_SIZE_X / 2 + 3), static_cast<float>(CHUNK_SIZE_Y / 2 + 40), static_cast<float>(CHUNK_SIZE_Z / 2 + 3)));
	_chunkManager.init();
	if (_atlas.load("./resources/textures/texture_atlas.bmp", 8) == false)
	{
		std::cerr << "Uh oh no atlas we lost :/" << std::endl;
	}
	_renderer.init();
}

void	VoxEngine::mainLoop()
{

	Shader	chunkShader("./resources/shaders/chunk.vert", "./resources/shaders/chunk.frag");
	#ifdef FPS
	int frame = 0;
	float time = glfwGetTime();
	#endif
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_input.handleKeys();
		Window::update();
		_chunkManager.update();
		_renderer.update();
		_renderer.render();

		glfwSwapBuffers(Window::get_window());
		glfwPollEvents();
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

void	VoxEngine::input()
{

}

void	VoxEngine::cleanup()
{
	_chunkManager.cleanup();
	_renderer.cleanup();
	_atlas.del();
	glfwTerminate();
}

void			VoxEngine::updateFrustum(const mlm::mat4 &projection, const mlm::mat4 &view)
{
	if (!_updateFrustum)
		return ;
	_frustum.update(projection * view);
	_updateFrustum = false;
}

Camera	&VoxEngine::getCamera()
{
	return (_camera);
}

Input	&VoxEngine::getInput()
{
	return (_input);
}

ChunkManager	&VoxEngine::getManager()
{
	return (_chunkManager);
}

Atlas			&VoxEngine::getAtlas()
{
	return (_atlas);
}

Frustum			&VoxEngine::getFrustum()
{
	return (_frustum);
}

void			VoxEngine::setFrustumUpdate()
{
	_updateFrustum = true;
}