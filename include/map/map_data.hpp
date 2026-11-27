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
	int doorTexture;			// 'D' (0 = sem linha "door <id>")
	int lockedTexture;			// 'L' (0 = sem linha "locked <id>")
	int exitTexture;			// 'X' (0 = sem linha "exit <id>")
	std::string nextMap;		// como escrito em "next <arquivo>" (vazio = fim do jogo)
	std::string nextMapPath;	// nextMap resolvido a partir da pasta do mapa (loadMapFile)

	// Linha do arquivo onde cada diretiva apareceu (0 = ausente, usa o padrão).
	int wallLine;
	int symbolLines[9];
	int floorLine;
	int ceilingLine;
	int doorLine;
	int lockedLine;
	int exitLine;
	int nextLine;

	std::vector<std::string> rows;	// grade como no arquivo
	std::vector<int> rowLines;		// linha do arquivo de cada linha da grade

	int spawnCol;	// primeiro 'P' (-1 = sem spawn)
	int spawnRow;

	MapData();
};

// Orientação de uma porta: em qual eixo a folha se estende (paredes nos dois lados desse eixo).
enum MapDoorAxis
{
	DoorAxisNone,
	DoorAxisX,	// paredes à esquerda e à direita (coluna-1 e coluna+1)
	DoorAxisZ	// paredes acima e abaixo (linha-1 e linha+1)
};

// '#' ou '1'-'9'
bool isMapWallCell(char cell);
// 'X': interruptor de saída (sólido)
bool isMapExitCell(char cell);
// Bloqueia sempre: paredes e 'X'
bool isMapSolidCell(char cell);
// 'D' porta, 'L' porta trancada
bool isMapDoorCell(char cell);
// Letras maiúsculas que não são 'P', 'D', 'L', 'X': itens de data/items.txt
bool isMapItemCell(char cell);
// Células aceitas na grade: paredes, '.', ' ', 'P', portas, 'X' e letras de item
bool isMapGridChar(char cell);

// Célula da grade ou '\0' fora dela.
char mapCellAt(const MapData& map, int col, int row);
// Textura de uma célula sólida ou porta ('#', '1'-'9', 'D', 'L', 'X'), com os padrões de fallback.
int mapWallTextureFor(const MapData& map, char cell);
MapDoorAxis mapDoorAxis(const MapData& map, int col, int row);

#endif
