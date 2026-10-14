#ifndef GAME_TRANSFORM_HPP
# define GAME_TRANSFORM_HPP

# include <glm/glm.hpp>

struct Transform
{
	glm::vec3 position;
	glm::vec3 scale;

	Transform()
		: position(0.0f),
		  scale(1.0f)
	{}
};

#endif
