#ifndef PLATFORM_WINDOW_HPP
# define PLATFORM_WINDOW_HPP

# include <SDL.h>

# include <vector>

struct Resolution
{
	int width;
	int height;

	Resolution(int width = 0, int height = 0)
		: width(width), height(height)
	{}

	bool operator==(const Resolution& other) const
	{
		return this->width == other.width && this->height == other.height;
	}
};

enum DisplayMode
{
	DisplayWindowed,	// janela comum, com borda, no tamanho escolhido
	DisplayBorderless,	// janela sem borda do tamanho da tela (usa a resolução do monitor)
	DisplayFullscreen	// tela cheia exclusiva, troca a resolução do monitor
};

class Window
{
	private:
		SDL_Window* window;
		SDL_GLContext glContext;
		bool sdlReady;
		DisplayMode mode;

	public:
		Window(Resolution size, DisplayMode mode);
		~Window();

		void swap();
		// Tamanho em pixels da área de desenho (glViewport, glScissor).
		int width() const;
		int height() const;
		// Tamanho em coordenadas de janela (as do mouse). Difere dos pixels em telas HiDPI.
		int windowWidth() const;
		int windowHeight() const;
		SDL_Window* sdlWindow() const;

		DisplayMode displayMode() const;
		Resolution desktopResolution() const;
		// Resoluções que cabem no monitor, da menor para a maior.
		std::vector<Resolution> availableResolutions() const;
		// Troca tamanho e modo. Retorna false (e volta ao modo anterior) se o sistema recusar.
		bool applyVideo(Resolution size, DisplayMode mode);
};

#endif
