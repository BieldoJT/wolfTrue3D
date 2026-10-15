#include "platform/input.hpp"

#include <SDL.h>

namespace
{
	bool isCtrlKey(SDL_Keycode key)
	{
		return key == SDLK_LCTRL || key == SDLK_RCTRL;
	}
}

Input::Input()
	: window(0),
	  quit(false),
	  moveForward(false),
	  moveBack(false),
	  moveLeft(false),
	  moveRight(false),
	  mouseDeltaX(0.0f),
	  mouseDeltaY(0.0f),
	  mouseCaptured(false),
	  mouseX(0),
	  mouseY(0),
	  mouseDown(false),
	  menuLeft(false),
	  menuRight(false)
{}

void Input::attachWindow(SDL_Window* window)
{
	this->window = window;
}

// Modo relativo nativo do SDL: o cursor fica preso e escondido e o mouse
// entrega deltas ilimitados, sem depender da borda da janela.
void Input::applyRelativeMode()
{
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GetRelativeMouseState(0, 0);
}

void Input::captureMouse(bool capture)
{
	this->mouseCaptured = capture;
	this->mouseDeltaX = 0.0f;
	this->mouseDeltaY = 0.0f;
	if (capture)
		this->applyRelativeMode();
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_GetMouseState(&this->mouseX, &this->mouseY);
	}
}

void Input::poll()
{
	this->mouseDeltaX = 0.0f;
	this->mouseDeltaY = 0.0f;
	this->menuLeft = false;
	this->menuRight = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			this->quit = true;
		else if (event.type == SDL_WINDOWEVENT
			&& event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED
			&& this->mouseCaptured)
			this->applyRelativeMode();
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0
			&& event.key.keysym.sym == SDLK_ESCAPE)
			this->quit = true;
		else if (event.type == SDL_KEYDOWN)
		{
			SDL_Keycode key = event.key.keysym.sym;
			if (isCtrlKey(key) && event.key.repeat == 0)
				this->captureMouse(!this->mouseCaptured);
			else if (key == SDLK_LEFT)
				this->menuLeft = true;
			else if (key == SDLK_RIGHT)
				this->menuRight = true;
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (this->mouseCaptured)
			{
				this->mouseDeltaX += static_cast<float>(event.motion.xrel);
				this->mouseDeltaY += static_cast<float>(event.motion.yrel);
			}
			else
			{
				this->mouseX = event.motion.x;
				this->mouseY = event.motion.y;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			this->mouseDown = true;
			this->mouseX = event.button.x;
			this->mouseY = event.button.y;
		}
		else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
			this->mouseDown = false;
	}

	const Uint8* keys = SDL_GetKeyboardState(0);
	this->moveForward = keys[SDL_SCANCODE_W] != 0;
	this->moveBack = keys[SDL_SCANCODE_S] != 0;
	this->moveLeft = keys[SDL_SCANCODE_A] != 0;
	this->moveRight = keys[SDL_SCANCODE_D] != 0;
}
