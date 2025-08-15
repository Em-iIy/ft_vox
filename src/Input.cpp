/*
Created by: Emily (Em_iIy) Winnink
Created on: 01/08/2025
*/

#include "Input.hpp"
#include "VoxEngine.hpp"

void	KeyState::updateState(int state)
{
	prev = current;
	current = state;
}

bool	KeyState::isPressed() const
{
	return (current == GLFW_PRESS && prev == GLFW_RELEASE);
}

bool	KeyState::isDown() const
{
	return (current == GLFW_PRESS || current == GLFW_REPEAT);
}

bool	KeyState::isReleased() const
{
	return (current == GLFW_RELEASE && prev == GLFW_PRESS);
}

void	Input::init(GLFWwindow *window, const mlm::ivec2 &windowSize)
{
	lastX = static_cast<float>(windowSize.x) / 2.0f;
	lastY = static_cast<float>(windowSize.y) / 2.0f;
	firstMouse = true;
	_window = window;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetCursorPosCallback(window, mouseMovementCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetKeyCallback(window, keyCallback);
}


void	Input::resizeCallback(GLFWwindow *window, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
	void	*ptr = glfwGetWindowUserPointer(window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);
	pEngine->set_update_flags(Window::SIZE);
}

void	Input::mouseMovementCallback(GLFWwindow *window, double inXPos, double inYPos)
{
	void	*ptr = glfwGetWindowUserPointer(window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);

	Input &input = pEngine->getInput();
	float xPos = static_cast<float>(inXPos);
	float yPos = static_cast<float>(inYPos);
	if (input.firstMouse)
	{
		input.lastX = xPos;
		input.lastY = yPos;
		input.firstMouse = false;
	}
	float	xOffset = xPos - input.lastX;
	float	yOffset = input.lastY - yPos;
	input.lastX = xPos;
	input.lastY = yPos;

	pEngine->getCamera().processMouseMovement(xOffset, yOffset);
}

void	Input::mouseScrollCallback(GLFWwindow *window, [[maybe_unused]] double xOffset, double yOffset)
{
	void	*ptr = glfwGetWindowUserPointer(window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);
	pEngine->getCamera().processMouseScroll(yOffset);
}

void	Input::handleKeys()
{
	void	*ptr = glfwGetWindowUserPointer(_window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);
	Camera		&camera = pEngine->getCamera();
	if (keys[GLFW_KEY_W].isDown())
		camera.processKeyboard(Camera::FORWARD, (1.f / 60.f));
	if (keys[GLFW_KEY_A].isDown())
		camera.processKeyboard(Camera::LEFT, (1.f / 60.f));
	if (keys[GLFW_KEY_S].isDown())
		camera.processKeyboard(Camera::BACKWARD, (1.f / 60.f));
	if (keys[GLFW_KEY_D].isDown())
		camera.processKeyboard(Camera::RIGHT, (1.f / 60.f));
	if (keys[GLFW_KEY_SPACE].isDown())
		camera.processKeyboard(Camera::UP, (1.f / 60.f));
	if (keys[GLFW_KEY_LEFT_SHIFT].isDown())
		camera.processKeyboard(Camera::DOWN, (1.f / 60.f));
	
	if (keys[GLFW_KEY_TAB].isPressed() && 0)
	{
		std::cout << "before: " << std::boolalpha << wireFrameMode << std::noboolalpha << std::endl;
		wireFrameMode = !wireFrameMode;
		std::cout << "after: " << std::boolalpha << wireFrameMode << std::noboolalpha << std::endl;
		if (wireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (keys[GLFW_KEY_X].isPressed())
	{
		KeyState &key = keys[GLFW_KEY_X];
		std::cout << key.current << " " << key.prev << std::endl;
	}

	if (keys[GLFW_KEY_ESCAPE].isPressed())
		glfwSetWindowShouldClose(_window, GLFW_TRUE);
}


void	Input::keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
	void	*ptr = glfwGetWindowUserPointer(window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);

	Input &input = pEngine->getInput();
	input.keys[key].updateState(action);
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		input.wireFrameMode = !input.wireFrameMode;
		if (input.wireFrameMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

