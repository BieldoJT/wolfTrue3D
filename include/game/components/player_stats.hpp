#ifndef GAME_PLAYER_STATS_HPP
# define GAME_PLAYER_STATS_HPP

// Vida, munição e chave do jogador. Vida e munição passam de fase; a chave não.
struct PlayerStats
{
	static const int MAX_HEALTH = 100;
	static const int MAX_AMMO = 99;

	int health;
	int ammo;
	bool hasKey;

	PlayerStats()
		: health(MAX_HEALTH), ammo(8), hasKey(false)
	{}
};

#endif
