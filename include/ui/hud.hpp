#ifndef UI_HUD_HPP
# define UI_HUD_HPP

class Window;
struct PlayerStats;
struct GameEvents;

// HUD mínimo: barra com vida, munição e chave, mira, mensagens temporárias e tela de fim.
class Hud
{
	public:
		void draw(Window& window, const PlayerStats& stats, const GameEvents& events);
		void drawGameOver(Window& window);
};

#endif
