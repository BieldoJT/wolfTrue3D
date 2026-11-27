#ifndef PLATFORM_INPUT_HPP
# define PLATFORM_INPUT_HPP

struct SDL_Window;

class Input
{
	private:
		SDL_Window* window;

		void applyRelativeMode();

	public:
		bool quit;
		bool moveForward;
		bool moveBack;
		bool moveLeft;
		bool moveRight;
		float mouseDeltaX;
		float mouseDeltaY;
		bool mouseCaptured;
		int mouseX;
		int mouseY;
		bool mouseDown;
		bool menuLeft;
		bool menuRight;
		bool use;		// E ou espaço, só no quadro em que foi apertado
		bool confirm;	// Enter, só no quadro em que foi apertado

		Input();

		void attachWindow(SDL_Window* window);
		void captureMouse(bool capture);
		void poll();
};

#endif
