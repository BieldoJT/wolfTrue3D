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
	  ignoreWarp(false),
	  ignoreWarpX(0),
	  ignoreWarpY(0),
	  quit(false),
	  moveForward(false),
	  moveBack(false),
	  moveLeft(false),
	  moveRight(false),
	  mouseDeltaX(0.0f),
	  mouseDeltaY(0.0f),
	  mouseCaptured(false)
{}

void Input::attachWindow(SDL_Window* window)
{
	this->window = window;
}

void Input::applyRelativeMode()
{
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1");
	SDL_ShowCursor(SDL_DISABLE);
	if (this->window)
		SDL_SetWindowGrab(this->window, SDL_TRUE);
	if (SDL_GetRelativeMouseMode() == SDL_TRUE)
		SDL_SetRelativeMouseMode(SDL_FALSE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Input::centerCursor(bool discardMotion)
{
	if (!this->window)
		return;

	int w = 0;
	int h = 0;
	SDL_GetWindowSize(this->window, &w, &h);
	if (w < 1 || h < 1)
		return;

	int cx = w / 2;
	int cy = h / 2;
	int mx = cx;
	int my = cy;
	SDL_GetMouseState(&mx, &my);
	this->ignoreWarp = true;
	this->ignoreWarpX = cx - mx;
	this->ignoreWarpY = cy - my;
	SDL_WarpMouseInWindow(this->window, cx, cy);
	SDL_PumpEvents();
	if (discardMotion)
	{
		SDL_FlushEvent(SDL_MOUSEMOTION);
		return;
	}

	SDL_Event queued[32];
	int count = SDL_PeepEvents(queued, 32, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION);
	if (count < 0)
		count = 0;
	for (int i = 0; i < count; ++i)
	{
		if (queued[i].motion.xrel == this->ignoreWarpX
			&& queued[i].motion.yrel == this->ignoreWarpY)
			continue;
		SDL_PushEvent(&queued[i]);
	}
}

void Input::captureMouse(bool capture)
{
	this->mouseCaptured = capture;
	if (capture)
	{
		this->applyRelativeMode();
		this->centerCursor(true);
	}
	else
	{
		this->ignoreWarp = false;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		if (this->window)
			SDL_SetWindowGrab(this->window, SDL_FALSE);
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void Input::poll()
{
	this->mouseDeltaX = 0.0f;
	this->mouseDeltaY = 0.0f;
	bool dropWarp = this->ignoreWarp;
	int dropWarpX = this->ignoreWarpX;
	int dropWarpY = this->ignoreWarpY;
	this->ignoreWarp = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			this->quit = true;
		else if (event.type == SDL_WINDOWEVENT
			&& event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED
			&& this->mouseCaptured)
		{
			this->mouseDeltaX = 0.0f;
			this->mouseDeltaY = 0.0f;
			this->applyRelativeMode();
			this->centerCursor(true);
		}
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				this->quit = true;
			else if (isCtrlKey(event.key.keysym.sym))
			{
				this->captureMouse(!this->mouseCaptured);
				this->mouseDeltaX = 0.0f;
				this->mouseDeltaY = 0.0f;
			}
		}
		else if (event.type == SDL_MOUSEMOTION && this->mouseCaptured)
		{
			if (dropWarp
				&& event.motion.xrel == dropWarpX
				&& event.motion.yrel == dropWarpY)
			{
				dropWarp = false;
				continue;
			}
			this->mouseDeltaX += static_cast<float>(event.motion.xrel);
			this->mouseDeltaY += static_cast<float>(event.motion.yrel);
		}
	}

	if (this->mouseCaptured)
	{
		if (SDL_GetRelativeMouseMode() != SDL_TRUE)
			this->applyRelativeMode();
		this->centerCursor(false);
	}

	const Uint8* keys = SDL_GetKeyboardState(0);
	this->moveForward = keys[SDL_SCANCODE_W] != 0;
	this->moveBack = keys[SDL_SCANCODE_S] != 0;
	this->moveLeft = keys[SDL_SCANCODE_A] != 0;
	this->moveRight = keys[SDL_SCANCODE_D] != 0;
}
