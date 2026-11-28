#include "app/settings.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
	const float MIN_SENSITIVITY = 0.01f;
	const float MAX_SENSITIVITY = 0.30f;

	bool readMode(const std::string& text, DisplayMode& out)
	{
		if (text == "window")
			out = DisplayWindowed;
		else if (text == "borderless")
			out = DisplayBorderless;
		else if (text == "fullscreen")
			out = DisplayFullscreen;
		else
			return false;
		return true;
	}
}

Settings::Settings()
	: resolution(1280, 720), mode(DisplayWindowed), mouseSensitivity(0.05f)
{}

const char* displayModeKey(DisplayMode mode)
{
	if (mode == DisplayBorderless)
		return "borderless";
	if (mode == DisplayFullscreen)
		return "fullscreen";
	return "window";
}

Settings loadSettings(const std::string& path)
{
	Settings settings;
	std::ifstream file(path.c_str());
	if (!file)
		return settings;	// primeira execução: padrão

	std::string line;
	int lineNo = 0;
	while (std::getline(file, line))
	{
		++lineNo;
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty() || (line.size() >= 2 && line[0] == '/' && line[1] == '/'))
			continue;

		std::istringstream iss(line);
		std::string key;
		iss >> key;
		bool ok = false;
		if (key == "resolution")
		{
			int w = 0;
			int h = 0;
			ok = (iss >> w >> h) && w >= 320 && h >= 240;
			if (ok)
				settings.resolution = Resolution(w, h);
		}
		else if (key == "mode")
		{
			std::string value;
			DisplayMode mode = DisplayWindowed;
			ok = (iss >> value) && readMode(value, mode);
			if (ok)
				settings.mode = mode;
		}
		else if (key == "sensitivity")
		{
			float value = 0.0f;
			ok = (iss >> value) && value >= MIN_SENSITIVITY && value <= MAX_SENSITIVITY;
			if (ok)
				settings.mouseSensitivity = value;
		}
		if (!ok)
			std::cerr << path << ":" << lineNo << ": aviso: opção inválida ignorada: " << line << std::endl;
	}
	return settings;
}

bool saveSettings(const std::string& path, const Settings& settings)
{
	std::ofstream file(path.c_str());
	if (!file)
	{
		std::cerr << "Não foi possível gravar " << path << std::endl;
		return false;
	}
	file << "// Opções do jogo (geradas pelo menu de opções, Ctrl)\n"
		<< "resolution " << settings.resolution.width << " " << settings.resolution.height << "\n"
		<< "mode " << displayModeKey(settings.mode) << "\n"
		<< "sensitivity " << settings.mouseSensitivity << "\n";
	return static_cast<bool>(file);
}
