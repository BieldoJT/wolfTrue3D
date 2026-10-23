#include "game/systems/render_system.hpp"
#include "game/scene.hpp"
#include "platform/window.hpp"
#include "render/shader.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

RenderSystem::RenderSystem()
	: missingTextureId(-1)
{}

void RenderSystem::draw(Scene& scene, Window& window, Shader& shader, TextureCatalog& textures)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Entity* cameraEntity = scene.cameraEntity();
	if (!cameraEntity || !cameraEntity->hasTransform)
		return;

	Camera& camera = cameraEntity->camera;
	glm::vec3 position = cameraEntity->transform.position;
	float yawRad = glm::radians(camera.yaw);
	float pitchRad = glm::radians(camera.pitch);
	glm::vec3 front(
		cos(yawRad) * cos(pitchRad),
		sin(pitchRad),
		sin(yawRad) * cos(pitchRad));
	front = glm::normalize(front);
	// up = worldUp perpendicular a front: continuo em pitch ±90 (sem degenerar)
	glm::vec3 up(
		-sin(pitchRad) * cos(yawRad),
		cos(pitchRad),
		-sin(pitchRad) * sin(yawRad));

	int w = window.width();
	int h = window.height();
	if (h < 1)
		h = 1;
	float aspect = static_cast<float>(w) / static_cast<float>(h);

	glm::mat4 view = glm::lookAt(position, position + front, up);
	glm::mat4 projection = glm::perspective(glm::radians(camera.fovDegrees), aspect, 0.1f, 100.0f);

	shader.use();
	shader.setInt("ourTexture", 0);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	std::vector<Entity>& entities = scene.getEntities();
	for (std::size_t i = 0; i < entities.size(); ++i)
	{
		Entity& entity = entities[i];
		if (!entity.hasTransform || !entity.hasMeshRenderer || !entity.meshRenderer.mesh)
			continue;

		Texture* texture = textures.find(entity.meshRenderer.textureId);
		if (!texture)
		{
			if (this->missingTextureId != entity.meshRenderer.textureId)
			{
				this->missingTextureId = entity.meshRenderer.textureId;
				std::cerr << "Textura id " << entity.meshRenderer.textureId << " não está no catálogo" << std::endl;
			}
			continue;
		}

		glm::mat4 model(1.0f);
		model = glm::translate(model, entity.transform.position);
		model = glm::scale(model, entity.transform.scale);
		shader.setMat4("model", model);
		texture->bind();
		entity.meshRenderer.mesh->draw();
	}
}
