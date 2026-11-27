#ifndef GAME_PICKUP_SYSTEM_HPP
# define GAME_PICKUP_SYSTEM_HPP

class Scene;

// Coleta itens quando o jogador passa por cima e aplica o efeito em PlayerStats.
class PickupSystem
{
	public:
		void update(Scene& scene);
};

#endif
