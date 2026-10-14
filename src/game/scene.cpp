#include "game/scene.hpp"

Scene::Scene()
{
	Mesh::uploadUnitCube(this->cube);
}

void Scene::reserve(std::size_t count)
{
	this->entities.reserve(count);
}

Entity& Scene::addEntity()
{
	this->entities.push_back(Entity());
	return this->entities.back();
}

std::vector<Entity>& Scene::getEntities()
{
	return this->entities;
}

Mesh& Scene::cubeMesh()
{
	return this->cube;
}

Entity* Scene::cameraEntity()
{
	for (std::size_t i = 0; i < this->entities.size(); ++i)
	{
		if (this->entities[i].hasCamera)
			return &this->entities[i];
	}
	return 0;
}
