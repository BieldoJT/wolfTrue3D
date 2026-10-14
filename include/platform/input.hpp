#ifndef PLATFORM_INPUT_HPP
# define PLATFORM_INPUT_HPP

struct SDL_Window;

class Input
{
	private:
		SDL_Window* window;
		bool ignoreWarp;
		int ignoreWarpX;
		int ignoreWarpY;

		void applyRelativeMode();
		void centerCursor(bool discardMotion);

	public:
		bool quit;
		bool moveForward;
		bool moveBack;
		bool moveLeft;
		bool moveRight;
		float mouseDeltaX;
		float mouseDeltaY;
		bool mouseCaptured;

		Input();

		void attachWindow(SDL_Window* window);
		void captureMouse(bool capture);
		void poll();
};

#endif
