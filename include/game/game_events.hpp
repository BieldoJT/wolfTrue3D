#ifndef GAME_GAME_EVENTS_HPP
# define GAME_GAME_EVENTS_HPP

# include <string>

// Recados entre sistemas e a App/HUD num quadro.
struct GameEvents
{
	std::string message;	// texto central temporário do HUD
	float messageTimer;
	bool exitRequested;		// jogador acionou o interruptor 'X'

	GameEvents()
		: messageTimer(0.0f), exitRequested(false)
	{}

	void show(const std::string& text)
	{
		this->message = text;
		this->messageTimer = 2.0f;
	}
};

#endif
