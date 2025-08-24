/*
Created by: Emily (Em_iIy) Winnink
Created on: 25/07/2025
*/

#pragma once

#include "glu/gl-utils.hpp"

const float	YAW 	= -90.0f;
const float	PITCH 	= 0.0f;
const float	SPEED 	= 50.0f;
const float	SENS 	= 0.1f;
const float	ZOOM 	= 60.0f;

class Camera {
	public:
		enum Direction {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN,
		};

		Camera(const mlm::vec3 &pos = mlm::vec3(0.0f), const mlm::vec3 &up = mlm::vec3(0.0f, 1.0f, 0.0f), const float yaw = YAW, const float pitch = PITCH);
		
		mlm::mat4	getViewMatrix() const;
		float		getZoom() const;

		void		setPos(const mlm::vec3 &pos);

		void		processKeyboard(Direction dir, float deltaTime);
		void		processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
		void		processMouseScroll(float yOffset);
	
	private:
		mlm::vec3	_pos;
		mlm::vec3	_worldUp;
		mlm::vec3	_front;
		mlm::vec3	_up;
		mlm::vec3	_right;

		float		_yaw;
		float		_pitch;

		float		_movementSpeed;
		float		_mouseSensitivity;
		float		_zoom;

		void		updateVectors();
};