#ifndef GAME_DOOR_HPP
# define GAME_DOOR_HPP

# include <glm/glm.hpp>

// Porta de uma célula 'D' ou 'L'. Desliza para dentro da parede ao abrir (DoorSystem).
struct Door
{
	enum State
	{
		Closed,
		Opening,
		Open,
		Closing
	};

	int col;
	int row;
	bool locked;			// 'L': precisa de chave
	bool alongX;			// folha se estende no eixo X (paredes à esquerda e à direita)
	float openAmount;		// 0 fechada, 1 aberta
	float timer;			// tempo aberta
	State state;
	glm::vec3 closedPosition;

	Door()
		: col(0), row(0), locked(false), alongX(true),
		  openAmount(0.0f), timer(0.0f), state(Closed), closedPosition(0.0f)
	{}
};

#endif
