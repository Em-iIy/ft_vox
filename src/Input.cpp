/*
Created by: Emily (Em_iIy) Winnink
Created on: 01/08/2025
*/

#include "Input.hpp"
#include "VoxEngine.hpp"

#include <algorithm>

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

void	Input::addOnPressCallback(int key, InputCallback callback)
{
	if (std::find(monitoredKeys.begin(), monitoredKeys.end(), key) == monitoredKeys.end())
		monitoredKeys.push_back(key);
	onPressCallbacks[key].push_back(callback);
}

void	Input::onPress(int key)
{
	std::vector<InputCallback>	&cb = onPressCallbacks[key];
	for (InputCallback &callback : cb)
		callback();
}

void	Input::addOnReleaseCallback(int key, InputCallback callback)
{
	if (std::find(monitoredKeys.begin(), monitoredKeys.end(), key) == monitoredKeys.end())
		monitoredKeys.push_back(key);
	onReleaseCallbacks[key].push_back(callback);
}

void	Input::onRelease(int key)
{
	std::vector<InputCallback>	&cb = onReleaseCallbacks[key];
	for (InputCallback &callback : cb)
		callback();
}

void	Input::addOnDownCallback(int key, InputCallback callback)
{
	if (std::find(monitoredKeys.begin(), monitoredKeys.end(), key) == monitoredKeys.end())
		monitoredKeys.push_back(key);
	onDownCallbacks[key].push_back(callback);
}

void	Input::onDown(int key)
{
	std::vector<InputCallback>	&cb = onDownCallbacks[key];
	for (InputCallback &callback : cb)
		callback();
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
	pEngine->setFrustumUpdate();


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
	pEngine->setFrustumUpdate();
	pEngine->getCamera().processMouseScroll(yOffset);
}

void	Input::handleKeys()
{
	for (int key : monitoredKeys)
		if (keys[key].isDown())
			onDown(key);
}

// Should be in the window or engine...
void	Input::toggleWireFrame()
{
	wireFrameMode = !wireFrameMode;
	glPolygonMode(GL_FRONT_AND_BACK, wireFrameMode ? GL_LINE : GL_FILL);
}

void	Input::keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
	void	*ptr = glfwGetWindowUserPointer(window);
	if (!ptr)
		return ;
	VoxEngine *pEngine = static_cast<VoxEngine *>(ptr);

	Input &input = pEngine->getInput();
	switch (action)
	{
	case GLFW_PRESS:
		input.onPress(key);
		break;
	case GLFW_RELEASE:
		input.onRelease(key);
		break;
	case GLFW_REPEAT:
		break;
	default:
		break;
	}
	input.keys[key].updateState(action);
}

