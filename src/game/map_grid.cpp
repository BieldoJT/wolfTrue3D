#include "game/map_grid.hpp"

#include <cmath>

MapGrid::MapGrid()
	: cellSize(1.0f)
{}

namespace
{
	int cellAt(float position, float cellSize)
	{
		return static_cast<int>(std::floor((position / cellSize) + 0.5f));
	}
}

bool MapGrid::overlapsWall(float x, float z, float radius) const
{
	if (this->cellSize <= 0.0f)
		return true;
	if (radius < 0.0f)
		radius = 0.0f;

	int col0 = cellAt(x - radius, this->cellSize);
	int col1 = cellAt(x + radius, this->cellSize);
	int row0 = cellAt(z - radius, this->cellSize);
	int row1 = cellAt(z + radius, this->cellSize);

	for (int row = row0; row <= row1; ++row)
	{
		if (row < 0 || row >= static_cast<int>(this->rows.size()))
			return true;
		const std::string& cells = this->rows[static_cast<std::size_t>(row)];
		for (int col = col0; col <= col1; ++col)
		{
			if (col < 0 || col >= static_cast<int>(cells.size()))
				return true;
			if (cells[static_cast<std::size_t>(col)] == '#')
				return true;
		}
	}
	return false;
}
