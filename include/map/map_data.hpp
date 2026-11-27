#ifndef MAP_MAP_DATA_HPP
# define MAP_MAP_DATA_HPP

# include <string>
# include <vector>

// Conteúdo de um mapa já lido do arquivo, sem nada de engine (jogo e editor usam o mesmo).
struct MapData
{
	int wallTexture;			// textura de '#'
	int symbolTextures[9];		// textura de '1'-'9' (0 = sem linha "wall N <id>")
	int floorTexture;
	int ceilingTexture;

	// Linha do arquivo onde cada diretiva apareceu (0 = ausente, usa o padrão).
	int wallLine;
	int symbolLines[9];
	int floorLine;
	int ceilingLine;

	std::vector<std::string> rows;	// grade como no arquivo
	std::vector<int> rowLines;		// linha do arquivo de cada linha da grade

	int spawnCol;	// primeiro 'P' (-1 = sem spawn)
	int spawnRow;

	MapData();
};

// '#' ou '1'-'9'
bool isMapWallCell(char cell);
// Células aceitas na grade: paredes, '.', ' ' e 'P'
bool isMapGridChar(char cell);
// Textura de uma célula de parede ('#' ou símbolo; símbolo sem "wall N" cai na de '#').
int mapWallTextureFor(const MapData& map, char cell);

#endif
