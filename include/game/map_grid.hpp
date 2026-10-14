#ifndef GAME_MAP_GRID_HPP
# define GAME_MAP_GRID_HPP

# include <string>
# include <vector>

struct MapGrid
{
	float cellSize;
	std::vector<std::string> rows;

	MapGrid();

	bool overlapsWall(float x, float z, float radius) const;
};

#endif
