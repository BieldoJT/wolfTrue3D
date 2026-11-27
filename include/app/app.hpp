#ifndef APP_APP_HPP
# define APP_APP_HPP

# include "app/settings.hpp"
# include "platform/window.hpp"
# include "platform/input.hpp"
# include "render/shader.hpp"
# include "render/texture.hpp"
# include "game/scene.hpp"
# include "game/systems/camera_system.hpp"
# include "game/systems/door_system.hpp"
# include "game/systems/interaction_system.hpp"
# include "game/systems/map_build_system.hpp"
# include "game/systems/pickup_system.hpp"
# include "game/systems/render_system.hpp"
# include "map/catalog.hpp"
# include "ui/hud.hpp"
# include "ui/options_menu.hpp"

# include <map>
# include <string>

class App
{
	private:
		Settings settings;	// antes de window: a janela nasce com a resolução/modo salvos
		Window window;
		Input input;
		TextureCatalog textures;
		std::map<char, ItemDef> items;
		Shader shader;
		Scene scene;
		MapBuildSystem mapBuild;
		CameraSystem cameraSystem;
		InteractionSystem interactionSystem;
		DoorSystem doorSystem;
		PickupSystem pickupSystem;
		RenderSystem renderSystem;
		OptionsMenu optionsMenu;
		Hud hud;
		std::string firstMapPath;
		std::string nextMapPath;	// "next" do mapa atual (vazio = fim do jogo)
		bool menuOpen;
		bool gameOver;

		// Lê, valida e monta um mapa. Vida e munição vêm de stats; a chave sempre zera.
		bool loadLevel(const std::string& mapPath, const PlayerStats& stats);
		void finishLevel();
		// Menu de opções (Ctrl): abre/fecha junto com a captura do mouse.
		void updateMenu();
		void drawMenu();

	public:
		App(const std::string& mapPath);
		void run();
};

#endif
