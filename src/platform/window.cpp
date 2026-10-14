#include "platform/window.hpp"
#include "defines.h"

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

Window::Window()
	: window(0),
	  glContext(0),
	  sdlReady(false)
{
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1");

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL não pôde ser inicializado! SDL_Error: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Falha ao inicializar o SDL");
	}
	sdlReady = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	this->window = SDL_CreateWindow("Hello SDL2",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDGHT,
		HEIGHT,
		SDL_WINDOW_OPENGL);
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

	int w = width();
	int h = height();
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);

	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
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
	int w = WIDGHT;
	int h = HEIGHT;
	if (this->window)
		SDL_GetWindowSize(this->window, &w, &h);
	return w;
}

int Window::height() const
{
	int w = WIDGHT;
	int h = HEIGHT;
	if (this->window)
		SDL_GetWindowSize(this->window, &w, &h);
	return h;
}

SDL_Window* Window::sdlWindow() const
{
	return this->window;
}
