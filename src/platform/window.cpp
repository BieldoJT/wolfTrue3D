#include "platform/window.hpp"

#include <glad/glad.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace
{
	// Resoluções comuns oferecidas no menu (as que não cabem no monitor são filtradas).
	const int COMMON[][2] = {
		{640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1366, 768},
		{1440, 900}, {1600, 900}, {1680, 1050}, {1920, 1080}, {1920, 1200},
		{2560, 1080}, {2560, 1440}, {3440, 1440}, {3840, 2160}
	};

	bool smaller(const Resolution& a, const Resolution& b)
	{
		if (a.width != b.width)
			return a.width < b.width;
		return a.height < b.height;
	}
}

Window::Window(Resolution size, DisplayMode mode)
	: window(0),
	  glContext(0),
	  sdlReady(false),
	  mode(DisplayWindowed)
{
#ifdef __linux__
	// No WSLg o X11 (XWayland) não entrega mouse relativo de verdade: o cursor
	// bate na borda da janela. O Wayland nativo tem relative-pointer.
	// SDL_VIDEODRIVER definido pelo usuário continua tendo prioridade.
	SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
#endif

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL não pôde ser inicializado! SDL_Error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Falha ao inicializar o SDL");
	}
	sdlReady = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Nasce em janela; o modo salvo é aplicado depois que o contexto OpenGL existe.
	SDL_DisplayMode desktop;
	if (SDL_GetDesktopDisplayMode(0, &desktop) == 0)
	{
		size.width = std::min(size.width, desktop.w);
		size.height = std::min(size.height, desktop.h);
	}
	this->window = SDL_CreateWindow("wolfTrue3D",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		size.width,
		size.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!this->window)
	{
		std::cerr << "Janela não pôde ser criada! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		sdlReady = false;
		throw std::runtime_error("Falha ao criar a janela");
	}

	this->glContext = SDL_GL_CreateContext(this->window);
	if (!this->glContext)
	{
		std::cerr << "Contexto OpenGL não pôde ser criado! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(this->window);
		this->window = 0;
		SDL_Quit();
		sdlReady = false;
		throw std::runtime_error("Falha ao criar o contexto OpenGL");
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cerr << "Erro ao inicializar o GLAD" << std::endl;
		SDL_GL_DeleteContext(this->glContext);
		this->glContext = 0;
		SDL_DestroyWindow(this->window);
		this->window = 0;
		SDL_Quit();
		sdlReady = false;
		throw std::runtime_error("Falha ao inicializar o GLAD");
	}

	std::cout << "Driver de vídeo: " << SDL_GetCurrentVideoDriver() << std::endl;

	glEnable(GL_DEPTH_TEST);
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

	if (mode != DisplayWindowed && !this->applyVideo(size, mode))
		std::cerr << "Modo de tela salvo não suportado; usando janela" << std::endl;
	glViewport(0, 0, this->width(), this->height());
}

Window::~Window()
{
	if (this->glContext)
		SDL_GL_DeleteContext(this->glContext);
	if (this->window)
		SDL_DestroyWindow(this->window);
	if (this->sdlReady)
		SDL_Quit();
}

void Window::swap()
{
	SDL_GL_SwapWindow(this->window);
}

int Window::width() const
{
	int w = 1;
	int h = 1;
	if (this->window)
		SDL_GL_GetDrawableSize(this->window, &w, &h);
	return w;
}

int Window::height() const
{
	int w = 1;
	int h = 1;
	if (this->window)
		SDL_GL_GetDrawableSize(this->window, &w, &h);
	return h;
}

int Window::windowWidth() const
{
	int w = 1;
	int h = 1;
	if (this->window)
		SDL_GetWindowSize(this->window, &w, &h);
	return w;
}

int Window::windowHeight() const
{
	int w = 1;
	int h = 1;
	if (this->window)
		SDL_GetWindowSize(this->window, &w, &h);
	return h;
}

SDL_Window* Window::sdlWindow() const
{
	return this->window;
}

DisplayMode Window::displayMode() const
{
	return this->mode;
}

Resolution Window::desktopResolution() const
{
	int display = this->window ? SDL_GetWindowDisplayIndex(this->window) : 0;
	SDL_DisplayMode desktop;
	if (SDL_GetDesktopDisplayMode(display < 0 ? 0 : display, &desktop) != 0)
		return Resolution(this->windowWidth(), this->windowHeight());
	return Resolution(desktop.w, desktop.h);
}

std::vector<Resolution> Window::availableResolutions() const
{
	Resolution desktop = this->desktopResolution();
	std::vector<Resolution> list;
	for (std::size_t i = 0; i < sizeof(COMMON) / sizeof(COMMON[0]); ++i)
		list.push_back(Resolution(COMMON[i][0], COMMON[i][1]));

	int display = this->window ? SDL_GetWindowDisplayIndex(this->window) : 0;
	if (display < 0)
		display = 0;
	int count = SDL_GetNumDisplayModes(display);
	for (int i = 0; i < count; ++i)
	{
		SDL_DisplayMode m;
		if (SDL_GetDisplayMode(display, i, &m) == 0)
			list.push_back(Resolution(m.w, m.h));
	}
	list.push_back(desktop);

	std::vector<Resolution> result;
	for (std::size_t i = 0; i < list.size(); ++i)
	{
		if (list[i].width > desktop.width || list[i].height > desktop.height)
			continue;
		if (std::find(result.begin(), result.end(), list[i]) == result.end())
			result.push_back(list[i]);
	}
	std::sort(result.begin(), result.end(), smaller);
	return result;
}

bool Window::applyVideo(Resolution size, DisplayMode mode)
{
	if (!this->window)
		return false;
	DisplayMode previous = this->mode;
	bool ok = true;

	if (mode == DisplayWindowed)
	{
		ok = SDL_SetWindowFullscreen(this->window, 0) == 0;
		if (ok)
		{
			Resolution desktop = this->desktopResolution();
			SDL_SetWindowSize(this->window, std::min(size.width, desktop.width),
				std::min(size.height, desktop.height));
			SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	else if (mode == DisplayBorderless)
		ok = SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0;
	else
	{
		int display = SDL_GetWindowDisplayIndex(this->window);
		SDL_DisplayMode want;
		SDL_zero(want);
		want.w = size.width;
		want.h = size.height;
		SDL_DisplayMode closest;
		ok = SDL_GetClosestDisplayMode(display < 0 ? 0 : display, &want, &closest) != 0
			&& SDL_SetWindowDisplayMode(this->window, &closest) == 0
			&& SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN) == 0;
	}

	if (!ok)
	{
		std::cerr << "Falha ao trocar o modo de tela: " << SDL_GetError() << std::endl;
		SDL_SetWindowFullscreen(this->window, previous == DisplayWindowed ? 0
			: previous == DisplayBorderless ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN);
		return false;
	}
	this->mode = mode;
	return true;
}
