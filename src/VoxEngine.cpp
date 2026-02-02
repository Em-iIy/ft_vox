/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/07/2025
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include "ShaderManager.hpp"

#include "Frustum.hpp"

const mlm::ivec2	WINDOW_SIZE(1024, 1024);
// const mlm::ivec2	WINDOW_SIZE(3840, 2160);

#define FPS

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
	// glfwSwapInterval(0);
	glfwSetWindowUserPointer(Window::get_window(), this);
	_input.init(Window::get_window(), Window::get_size());

	// Reload all chunks?
	_input.addOnPressCallback(GLFW_KEY_R, [this]() {_chunkManager.unloadAll();});
	_input.addOnPressCallback(GLFW_KEY_R, []() {ShaderManager::reloadShaders();});

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
	_input.addOnPressCallback(GLFW_KEY_RIGHT_CONTROL, [this]() {_renderer.togglePause();});
	_input.addOnPressCallback(GLFW_KEY_RIGHT_CONTROL, [this]() {_sky.togglePause();});
	_input.addOnPressCallback(GLFW_KEY_KP_0, [this]() {_renderer.setLightingMode(0);});
	_input.addOnPressCallback(GLFW_KEY_KP_1, [this]() {_renderer.setLightingMode(1);});
	_input.addOnPressCallback(GLFW_KEY_KP_2, [this]() {_renderer.setLightingMode(2);});
	_input.addOnPressCallback(GLFW_KEY_KP_3, [this]() {_renderer.setLightingMode(3);});

	_input.addOnPressCallback(GLFW_KEY_LEFT, [this]() {_renderer.swapFrameBuffer(-1);});
	_input.addOnPressCallback(GLFW_KEY_RIGHT, [this]() {_renderer.swapFrameBuffer(1);});


	glfwSetCursorPos(Window::get_window(), WINDOW_SIZE.x / 2.0f, WINDOW_SIZE.y / 2.0f);

	if (_atlas.load() == false)
	{
		throw std::runtime_error("Uh oh no atlas we lost :/");
	}

	_sky.load(Settings::loadSky());
	
	_camera.setPos(mlm::vec3(static_cast<float>(CHUNK_SIZE_X / 2 + 3), static_cast<float>(CHUNK_SIZE_Y / 2 + 40), static_cast<float>(CHUNK_SIZE_Z / 2 + 3)));
	_chunkManager.init(Settings::loadChunkManager());

	_renderer.init();
}

void	VoxEngine::mainLoop()
{
	#ifdef FPS
	int frame = 0;
	float time = glfwGetTime();
	#endif
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_input.handleKeys();
		Window::update();
		_renderer.update(); // Maybe this order fixed the culling issue (test more!)
		updateFrustum(_renderer.getProjection(), _renderer.getView());
		updateShadowFrustum(_renderer.getLightProjection(), _renderer.getLightView());
		_chunkManager.update();
		_sky.update(Window::get_delta_time());

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

void	VoxEngine::updateFrustum(const mlm::mat4 &projection, const mlm::mat4 &view)
{
	if (!_updateFrustum)
		return ;
	_frustum.update(projection * view);
	_updateFrustum = false;
}

void	VoxEngine::updateShadowFrustum(const mlm::mat4 &projection, const mlm::mat4 &view)
{
	_shadowFrustum.update(projection * view);
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

Atlas	&VoxEngine::getAtlas()
{
	return (_atlas);
}

Frustum	&VoxEngine::getFrustum()
{
	return (_frustum);
}

Frustum	&VoxEngine::getShadowFrustum()
{
	return (_shadowFrustum);
}

Sky	&VoxEngine::getSky()
{
	return (_sky);
}

void	VoxEngine::setFrustumUpdate()
{
	_updateFrustum = true;
}
