#ifndef RENDER_MESH_HPP
# define RENDER_MESH_HPP

class Mesh
{
	private:
		unsigned int vao;
		unsigned int vbo;
		int vertexCount;

	public:
		Mesh();
		~Mesh();

		void upload(const float* vertices, int count);
		void draw() const;

		static void uploadUnitCube(Mesh& mesh);
		// Quad horizontal 1x1 em y=0; a textura se repete tilesU x tilesV vezes.
		static void uploadPlane(Mesh& mesh, float tilesU, float tilesV);
};

#endif
