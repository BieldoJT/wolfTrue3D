#ifndef GAME_MESH_RENDERER_HPP
# define GAME_MESH_RENDERER_HPP

class Mesh;

struct MeshRenderer
{
	Mesh* mesh;
	int textureId;

	MeshRenderer()
		: mesh(0),
		  textureId(0)
	{}
};

#endif
