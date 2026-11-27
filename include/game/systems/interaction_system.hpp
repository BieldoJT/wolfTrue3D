#ifndef GAME_INTERACTION_SYSTEM_HPP
# define GAME_INTERACTION_SYSTEM_HPP

class Scene;
class Input;

// Tecla de usar (E/espaço): abre a porta ou aciona o interruptor de saída à frente do jogador.
class InteractionSystem
{
	public:
		void update(Scene& scene, const Input& input);
};

#endif
