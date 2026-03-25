/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/03/2026
*/

#include "VoxEngine.hpp"
#include "Settings.hpp"
#include "ShaderManager.hpp"
#include "Logger.hpp"

VoxEngine::VoxEngine(): _chunkManager(*this), _renderer(*this, _chunkManager, _camera)
{}

VoxEngine::~VoxEngine()
{}

void	VoxEngine::_init()
{
	rng::seed();

	init_glfw();

	EngineDTO settings = Settings::loadEngine();

	_initWindow(settings);
	_initInput();
	_initResources(settings);
	_initComponents();
}

void	VoxEngine::_initWindow(EngineDTO &settings)
{
	Logger::info("Creating window");
	Window::create_window(
		"ft_vox",
		mlm::ivec2(static_cast<int>(settings.windowSettings.width),
		static_cast<int>(settings.windowSettings.height)),
		settings.windowSettings.fullscreen ? Window::FULL_SCREEN_WINDOWED : Window::WINDOWED
	);

	if (settings.windowSettings.vsync == false)
		glfwSwapInterval(0);
}

void	VoxEngine::_initInput()
{
	glfwSetWindowUserPointer(Window::get_window(), this);
	_input.init(Window::get_window(), Window::get_size());

	// Reload all chunks and shaders
	_input.addOnPressCallback(GLFW_KEY_R, [this]() {_chunkManager.unloadAll();});
	_input.addOnPressCallback(GLFW_KEY_R, []() {ShaderManager::reloadShaders();});

	// Camera movement
	_input.addOnDownCallback(GLFW_KEY_W, [this]() {_camera.processKeyboard(Camera::FORWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_S, [this]() {_camera.processKeyboard(Camera::BACKWARD, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_D, [this]() {_camera.processKeyboard(Camera::RIGHT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_A, [this]() {_camera.processKeyboard(Camera::LEFT, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_SPACE, [this]() {_camera.processKeyboard(Camera::UP, get_delta_time());});
	_input.addOnDownCallback(GLFW_KEY_LEFT_SHIFT, [this]() {_camera.processKeyboard(Camera::DOWN, get_delta_time());});

	// Sprinting
	_input.addOnPressCallback(GLFW_KEY_LEFT_CONTROL, [this]() {_camera.toggleSprint();});
	_input.addOnReleaseCallback(GLFW_KEY_LEFT_CONTROL, [this]() {_camera.toggleSprint();});

	// Block placing
	_input.addOnPressCallback(GLFW_KEY_1, [this]() {_player.setActiveBlock(Block::DIRT);});
	_input.addOnPressCallback(GLFW_KEY_2, [this]() {_player.setActiveBlock(Block::GRASS);});
	_input.addOnPressCallback(GLFW_KEY_3, [this]() {_player.setActiveBlock(Block::STONE);});
	_input.addOnPressCallback(GLFW_KEY_4, [this]() {_player.setActiveBlock(Block::SAND);});
	_input.addOnPressCallback(GLFW_KEY_5, [this]() {_player.setActiveBlock(Block::WATER);});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_LEFT, [this]() {_chunkManager.placeBlock(_player.getActiveBlock());});
	_input.addOnDownCallback(GLFW_MOUSE_BUTTON_MIDDLE, [this]() {_chunkManager.setBlock(_camera.getPos(), _player.getActiveBlock());});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_RIGHT, [this]() {_chunkManager.deleteBlock();});

	// Random other key inputs
	_input.addOnPressCallback(GLFW_KEY_ESCAPE, std::bind(glfwSetWindowShouldClose, get_window(), GLFW_TRUE));
	_input.addOnPressCallback(GLFW_KEY_TAB, [this]() {_input.toggleWireFrame();});
	_input.addOnPressCallback(GLFW_KEY_RIGHT_CONTROL, [this]() {_sky.togglePause();});

	mlm::vec2	size = static_cast<mlm::vec2>(Window::get_size());
	glfwSetCursorPos(Window::get_window(), size.x / 2.0f, size.y / 2.0f);
}

void	VoxEngine::_initResources(EngineDTO &settings)
{
	_camera.setPos(mlm::vec3(static_cast<float>(CHUNK_SIZE_X / 2 + 3), static_cast<float>(CHUNK_SIZE_Y / 2 + 40), static_cast<float>(CHUNK_SIZE_Z / 2 + 3)));
	_camera.loadSettings(settings.cameraSettings);

	if (_atlas.load() == false)
	{
		throw std::runtime_error("Uh oh no atlas we lost :/");
	}

	_sky.load(Settings::loadSky());
}

void	VoxEngine::_initComponents()
{
	_chunkManager.init(Settings::loadChunkManager());
	_renderer.init();
}
