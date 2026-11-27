#ifndef GAME_MAP_GRID_HPP
# define GAME_MAP_GRID_HPP

# include <set>
# include <string>
# include <utility>
# include <vector>

struct MapGrid
{
	float cellSize;
	std::vector<std::string> rows;
	std::set<std::pair<int, int> > openDoors;	// (coluna, linha) das portas abertas o bastante para passar

	MapGrid();

	// parede: '#' (textura padrão) ou '1'-'9' (textura por símbolo)
	static bool isWallCell(char cell);

	// Célula da posição no mundo (centro da célula = coordenada inteira * cellSize).
	int cellAt(float position) const;
	// Bloqueia: parede, 'X', célula fora da grade ou porta fechada.
	bool isSolid(int col, int row) const;
	char cell(int col, int row) const;	// '\0' fora da grade
	bool overlapsWall(float x, float z, float radius) const;
	// Círculo do jogador encosta na célula?
	bool overlapsCell(float x, float z, float radius, int col, int row) const;
};

#endif
