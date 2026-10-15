#ifndef APP_APP_HPP
# define APP_APP_HPP

# include "platform/window.hpp"
# include "platform/input.hpp"
# include "render/shader.hpp"
# include "render/texture.hpp"
# include "game/scene.hpp"
# include "game/systems/camera_system.hpp"
# include "game/systems/map_build_system.hpp"
# include "game/systems/render_system.hpp"
# include "ui/sensitivity_menu.hpp"

class App
{
	private:
		Window window;
		Input input;
		TextureCatalog textures;
		Shader shader;
		Scene scene;
		MapBuildSystem mapBuild;
		CameraSystem cameraSystem;
		RenderSystem renderSystem;
		SensitivityMenu sensitivityMenu;

	public:
		App();
		void run();
};

#endif
