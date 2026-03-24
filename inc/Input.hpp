/*
Created by: Emily (Em_iIy) Winnink
Created on: 01/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include <array>
#include <vector>
#include <functional>

struct KeyState {
	int	current = GLFW_RELEASE;
	int	prev = GLFW_RELEASE;

	void	updateState(int state);
	bool	isPressed() const;
	bool	isDown() const;
	bool	isReleased() const;
};

class Input {
	public:
		using InputCallback = std::function<void()>;

		void											addOnPressCallback(int key, InputCallback callback);
		void											onPress(int key);
		void											addOnReleaseCallback(int key, InputCallback callback);
		void											onRelease(int key);
		void											addOnDownCallback(int key, InputCallback callback);
		void											onDown(int key);

		void											init(GLFWwindow *window, const mlm::ivec2 &windowSize);

		void											handleKeys();

		void											toggleWireFrame();
		bool											getWireFrameMode();

		/*    Callbacks    */
		static void										resizeCallback(GLFWwindow *window, int x, int y);
		static void										mouseMovementCallback(GLFWwindow *window, double inXPos, double inYPos);
		static void										mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
		static void										mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void										keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

	private:
		std::array<KeyState, 1024>						_keys;
		std::vector<int>								_monitoredKeys;
		std::array<std::vector<InputCallback>, 1024>	_onPressCallbacks;
		std::array<std::vector<InputCallback>, 1024>	_onReleaseCallbacks;
		std::array<std::vector<InputCallback>, 1024>	_onDownCallbacks;

		float											_lastX = 0.0f;
		float											_lastY = 0.0f;
		bool											_firstMouse = true;

		bool											_wireFrameMode = false;
		GLFWwindow										*_window;
};
