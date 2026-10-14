#ifndef GAME_CAMERA_HPP
# define GAME_CAMERA_HPP

struct Camera
{
	float yaw;
	float pitch;
	float fovDegrees;
	float moveSpeed;
	float mouseSensitivity;

	Camera()
		: yaw(-90.0f),
		  pitch(0.0f),
		  fovDegrees(90.0f),
		  moveSpeed(2.5f),
		  mouseSensitivity(0.1f)
	{}
};

#endif
