/*
Created by: Emily (Em_iIy) Winnink
Created on: 25/07/2025
*/

#include "Camera.hpp"

Camera::Camera(const mlm::vec3 &pos, const mlm::vec3 &up, const float yaw, const float pitch):
	_pos(pos),
	_worldUp(up),
	_front(mlm::vec3(0.0f, 0.0f, -1.0f)),
	_yaw(yaw),
	_pitch(pitch),
	_movementSpeed(SPEED),
	_mouseSensitivity(SENS),
	_zoom(ZOOM)
{
	updateVectors();
}

mlm::mat4	Camera::getViewMatrix() const
{
	return (mlm::lookat(_front, _up));
}

void		Camera::processKeyboard(Direction dir, float deltaTime)
{
	float	velocity = _movementSpeed * deltaTime;
	if (_sprinting)
		velocity *= SPRINT_MULTIPLIER;
	mlm::vec3	worldFront;
	switch (dir)
	{
		case FORWARD:
			worldFront = mlm::cross(_worldUp, _right);
			_pos += worldFront * velocity;
			break ;
		case BACKWARD:
			worldFront = mlm::cross(_worldUp, _right);
			_pos -= worldFront * velocity;
			break ;
		case LEFT:
			_pos -= _right * velocity;
			break ;
		case RIGHT:
			_pos += _right * velocity;
			break ;
		case UP:
			_pos += _worldUp * velocity;
			break ;
		case DOWN:
			_pos -= _worldUp * velocity;
			break ;
		default:
			break;
	}
}

void		Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= _mouseSensitivity;
	yOffset *= _mouseSensitivity;

	_yaw += xOffset;
	_pitch += yOffset;

	if (constrainPitch)
	{
		if (_pitch > 89.0f)
			_pitch = 89.0f;
		else if (_pitch < -89.0f)
			_pitch = -89.0f;
	}

	updateVectors();
}

void		Camera::processMouseScroll(float yOffset)
{
	_zoom -= yOffset;
	if (_zoom > 90.0f)
		_zoom = 90.0f;
	else if (_zoom < 1.0f)
		_zoom = 1.0f;
	std::cout << _zoom << std::endl;
}

void		Camera::toggleSprint()
{
	_sprinting = !_sprinting;
}

void		Camera::updateVectors()
{
	_front = mlm::normalize(
		mlm::vec3(
			cos(mlm::radians(_yaw)) * cos(mlm::radians(_pitch)),
			sin(mlm::radians(_pitch)),
			sin(mlm::radians(_yaw)) * cos(mlm::radians(_pitch))
		)
	);

	_right = mlm::normalize(mlm::cross(_front, _worldUp));
	_up = mlm::cross(_right, _front);
}

float	Camera::getZoom() const
{
	return (_zoom);
}

const mlm::vec3	&Camera::getPos() const
{
	return (_pos);
}

void		Camera::setPos(const mlm::vec3 &pos)
{
	_pos = pos;
}
