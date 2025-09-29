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

VoxEngine::VoxEngine(): _chunkManager(*this)
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

	_input.addOnDownCallback(GLFW_KEY_1, [this]() {_chunkManager.setBlock(_camera.getPos(), Block::STONE);});
	_input.addOnDownCallback(GLFW_KEY_2, [this]() {_chunkManager.setBlock(_camera.getPos(), Block::WATER);});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_LEFT, [this]() {_chunkManager.placeBlock(Block::GRASS);});
	_input.addOnPressCallback(GLFW_MOUSE_BUTTON_RIGHT, [this]() {_chunkManager.deleteBlock();});

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

	Shader	chunkShader("./resources/shaders/chunk.vert", "./resources/shaders/chunk.frag");
	Shader	cubeShader("./resources/shaders/cube.vert", "./resources/shaders/cube.frag");

	std::vector<Vertex>		vertices = {
		{mlm::vec3(0.0f, 0.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 0.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 1.0f, 0.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 0.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 0.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(0.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
		{mlm::vec3(1.0f, 1.0f, 1.0f), mlm::vec3(0.0f), mlm::vec2(0.0f)},
	};
	std::vector<uint32_t>	indices = {
		0, 2, 1,
		1, 2, 3,
		4, 5, 6,
		5, 7, 6,
		0, 4, 6,
		0, 6, 2,
		1, 7, 5,
		1, 3, 7,
		2, 6, 7,
		2, 7, 3,
		1, 4, 0,
		1, 5, 4,
	};
	Mesh	cubeMesh(vertices, indices);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	#ifdef FPS
	int frame = 0;
	float time = glfwGetTime();
	#endif
	mlm::vec3	bgColor = mlm::vec3(0.4f, 0.7f, 0.9f);
	mlm::vec3	tempBgColor = bgColor;
	while (!glfwWindowShouldClose(Window::get_window()))
	{
		_input.handleKeys();
		Window::update();
		mlm::vec2	size = static_cast<mlm::vec2>(get_size());
		
		mlm::mat4	projection = mlm::perspective(_camera.getZoom(), size.x / size.y, .5f, 640.0f);
		chunkShader.use();
		chunkShader.set_mat4("projection", projection);

		mlm::mat4	view = _camera.getViewMatrix();
		chunkShader.set_mat4("view", view);

		auto block = _chunkManager.getBlockType(_camera.getPos());
		if (block.hasValue() && block.value() == Block::WATER)
		{
			chunkShader.set_float("uFogNear", 0.0f);
			tempBgColor = mlm::vec3(0.0f, 0.0f, 0.8f);
		}
		else
		{
			chunkShader.set_float("uFogNear", 120.0f);
			tempBgColor = bgColor;
		}

		chunkShader.set_float("uFogFar", 160.0f);
		chunkShader.set_vec3("uFogColor", tempBgColor);
		updateFrustum(projection, view);

		glClearColor(tempBgColor.x, tempBgColor.y, tempBgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		_atlas.bind();

		_chunkManager.update();
		_chunkManager.render(chunkShader);

		Expected<mlm::ivec3, bool>	rayWorldCoord = _chunkManager.castRayIncluding();
		if (rayWorldCoord.hasValue())
		{
			cubeShader.use();
			cubeShader.set_mat4("projection", projection);
			cubeShader.set_mat4("view", view);
			mlm::mat4	model(1.0f);
			mlm::vec3 pos = static_cast<mlm::vec3>(rayWorldCoord.value()) - _camera.getPos();
			model = mlm::translate(model, pos);
			cubeShader.set_mat4("model", model);
			cubeMesh.draw(cubeShader);
		}

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