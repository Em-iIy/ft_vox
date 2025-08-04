/*
Created by: Emily (Em_iIy) Winnink
Created on: 01/08/2025
*/

#pragma once

#include "glu/gl-utils.hpp"
#include <array>

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
		float	lastX = 0.0f;
		float	lastY = 0.0f;
		bool	firstMouse = true;
	
		std::array<KeyState, 1024>	keys;

		void	init(GLFWwindow *window, const mlm::ivec2 &windowSize);

		void	handleKeys();

		/*    Callbacks    */
		static void	resizeCallback(GLFWwindow *window, int x, int y);
		static void	mouseMovementCallback(GLFWwindow *window, double inXPos, double inYPos);
		static void	mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
		static void	keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

	private:
		GLFWwindow *_window;		
};