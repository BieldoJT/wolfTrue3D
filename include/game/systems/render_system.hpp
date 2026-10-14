#ifndef GAME_RENDER_SYSTEM_HPP
# define GAME_RENDER_SYSTEM_HPP

class Scene;
class Window;
class Shader;
class TextureCatalog;

class RenderSystem
{
	private:
		int missingTextureId;

	public:
		RenderSystem();
		void draw(Scene& scene, Window& window, Shader& shader, TextureCatalog& textures);
};

#endif
