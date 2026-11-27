#ifndef GAME_DOOR_SYSTEM_HPP
# define GAME_DOOR_SYSTEM_HPP

class Scene;

// Anima as portas (abrir, esperar, fechar) e libera a passagem no MapGrid.
class DoorSystem
{
	public:
		void update(Scene& scene, float dt);
};

#endif
