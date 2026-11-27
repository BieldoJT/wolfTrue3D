#ifndef MAP_MAP_MESSAGES_HPP
# define MAP_MAP_MESSAGES_HPP

# include <ostream>
# include <string>
# include <vector>

// Mensagem do parser/validador. Erro impede o carregamento; aviso só informa.
struct MapMessage
{
	enum Level
	{
		Warning,
		Error
	};

	Level level;
	int line;	// 0 = sem linha específica
	std::string text;

	MapMessage(Level level, int line, const std::string& text);
};

void addMapWarning(std::vector<MapMessage>& messages, int line, const std::string& text);
void addMapError(std::vector<MapMessage>& messages, int line, const std::string& text);
bool hasMapErrors(const std::vector<MapMessage>& messages);
bool hasMapWarnings(const std::vector<MapMessage>& messages);

// Formato "arquivo:linha: aviso|erro: texto", como compiladores.
void printMapMessages(std::ostream& out, const std::string& file, const std::vector<MapMessage>& messages);

#endif
