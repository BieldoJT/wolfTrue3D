#ifndef GAME_MESH_RENDERER_HPP
# define GAME_MESH_RENDERER_HPP

class Mesh;

struct MeshRenderer
{
	Mesh* mesh;
	int textureId;
	bool billboard;	// gira em Y para ficar de frente para a câmera (sprites)

	MeshRenderer()
		: mesh(0),
		  textureId(0),
		  billboard(false)
	{}
};

#endif
