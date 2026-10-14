#include "game/systems/camera_system.hpp"
#include "game/scene.hpp"
#include "platform/input.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CameraSystem::CameraSystem()
	: ignoreFirstMouse(true)
{}

void CameraSystem::update(Scene& scene, const Input& input, float dt)
{
	Entity* entity = scene.cameraEntity();
	if (!entity || !entity->hasTransform)
		return;

	Camera& camera = entity->camera;
	Transform& transform = entity->transform;

	if (input.mouseCaptured)
	{
		if (this->ignoreFirstMouse)
		{
			if (input.mouseDeltaX != 0.0f || input.mouseDeltaY != 0.0f)
				this->ignoreFirstMouse = false;
		}
		else
		{
			camera.yaw += input.mouseDeltaX * camera.mouseSensitivity;
			camera.pitch -= input.mouseDeltaY * camera.mouseSensitivity;
			if (camera.pitch > 90.0f)
				camera.pitch = 90.0f;
			if (camera.pitch < -90.0f)
				camera.pitch = -90.0f;
		}
	}

	glm::vec3 forward(
		cos(glm::radians(camera.yaw)),
		0.0f,
		sin(glm::radians(camera.yaw)));
	forward = glm::normalize(forward);
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::vec3 delta(0.0f);
	float distance = camera.moveSpeed * dt;
	if (input.moveForward)
		delta += forward * distance;
	if (input.moveBack)
		delta -= forward * distance;
	if (input.moveRight)
		delta += right * distance;
	if (input.moveLeft)
		delta -= right * distance;

	if (!entity->hasCollider)
	{
		transform.position += delta;
		return;
	}

	float radius = entity->collider.radius;
	MapGrid& grid = scene.grid();
	float nextX = transform.position.x + delta.x;
	if (!grid.overlapsWall(nextX, transform.position.z, radius))
		transform.position.x = nextX;
	float nextZ = transform.position.z + delta.z;
	if (!grid.overlapsWall(transform.position.x, nextZ, radius))
		transform.position.z = nextZ;
}
