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
	: quit(false),
	  moveForward(false),
	  moveBack(false),
	  moveLeft(false),
	  moveRight(false),
	  mouseDeltaX(0.0f),
	  mouseDeltaY(0.0f),
	  mouseCaptured(false)
{}

void Input::captureMouse(bool capture)
{
	this->mouseCaptured = capture;
	SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
	SDL_ShowCursor(capture ? SDL_DISABLE : SDL_ENABLE);
}

void Input::poll()
{
	this->mouseDeltaX = 0.0f;
	this->mouseDeltaY = 0.0f;
	bool eatMouse = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			this->quit = true;
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				this->quit = true;
			else if (isCtrlKey(event.key.keysym.sym))
			{
				this->captureMouse(!this->mouseCaptured);
				this->mouseDeltaX = 0.0f;
				this->mouseDeltaY = 0.0f;
				eatMouse = true;
			}
		}
		else if (event.type == SDL_MOUSEMOTION && this->mouseCaptured && !eatMouse)
		{
			// xrel > 0: direita. yrel > 0: baixo. O olhar aplica o sinal.
			this->mouseDeltaX += static_cast<float>(event.motion.xrel);
			this->mouseDeltaY += static_cast<float>(event.motion.yrel);
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(0);
	this->moveForward = keys[SDL_SCANCODE_W] != 0;
	this->moveBack = keys[SDL_SCANCODE_S] != 0;
	this->moveLeft = keys[SDL_SCANCODE_A] != 0;
	this->moveRight = keys[SDL_SCANCODE_D] != 0;
}
