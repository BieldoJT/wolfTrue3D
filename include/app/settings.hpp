#ifndef APP_SETTINGS_HPP
# define APP_SETTINGS_HPP

# include "platform/window.hpp"

# include <string>

// Opções do jogador guardadas em settings.txt (fora do git).
struct Settings
{
	Resolution resolution;
	DisplayMode mode;
	float mouseSensitivity;

	Settings();
};

// Arquivo ausente ou linha inválida: mantém o padrão e avisa no stderr. Nunca impede o jogo de abrir.
Settings loadSettings(const std::string& path);
bool saveSettings(const std::string& path, const Settings& settings);

const char* displayModeKey(DisplayMode mode);	// "window", "borderless", "fullscreen"

#endif
