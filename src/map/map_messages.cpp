#include "map/map_messages.hpp"

MapMessage::MapMessage(Level level, int line, const std::string& text)
	: level(level), line(line), text(text)
{}

void addMapWarning(std::vector<MapMessage>& messages, int line, const std::string& text)
{
	messages.push_back(MapMessage(MapMessage::Warning, line, text));
}

void addMapError(std::vector<MapMessage>& messages, int line, const std::string& text)
{
	messages.push_back(MapMessage(MapMessage::Error, line, text));
}

bool hasMapErrors(const std::vector<MapMessage>& messages)
{
	for (std::size_t i = 0; i < messages.size(); ++i)
		if (messages[i].level == MapMessage::Error)
			return true;
	return false;
}

bool hasMapWarnings(const std::vector<MapMessage>& messages)
{
	for (std::size_t i = 0; i < messages.size(); ++i)
		if (messages[i].level == MapMessage::Warning)
			return true;
	return false;
}

void printMapMessages(std::ostream& out, const std::string& file, const std::vector<MapMessage>& messages)
{
	for (std::size_t i = 0; i < messages.size(); ++i)
	{
		const MapMessage& message = messages[i];
		out << file;
		if (message.line > 0)
			out << ":" << message.line;
		out << ": " << (message.level == MapMessage::Error ? "erro" : "aviso")
			<< ": " << message.text << std::endl;
	}
}
