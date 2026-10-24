#ifndef GAME_MAP_GRID_HPP
# define GAME_MAP_GRID_HPP

# include <string>
# include <vector>

struct MapGrid
{
	float cellSize;
	std::vector<std::string> rows;

	MapGrid();

	// parede: '#' (textura padrão) ou '1'-'9' (textura por símbolo)
	static bool isWallCell(char cell);

	bool overlapsWall(float x, float z, float radius) const;
};

#endif
