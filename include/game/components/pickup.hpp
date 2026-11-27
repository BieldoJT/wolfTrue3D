#ifndef GAME_PICKUP_HPP
# define GAME_PICKUP_HPP

# include "map/catalog.hpp"

// Item coletável no chão (sprite billboard). Copia a definição de data/items.txt.
struct Pickup
{
	ItemDef item;
	bool collected;

	Pickup()
		: collected(false)
	{}
};

#endif
