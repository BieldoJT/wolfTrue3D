#ifndef PLATFORM_INPUT_HPP
# define PLATFORM_INPUT_HPP

class Input
{
	public:
		bool quit;
		bool moveForward;
		bool moveBack;
		bool moveLeft;
		bool moveRight;
		float mouseDeltaX;
		float mouseDeltaY;

		Input();

		void captureMouse(bool capture);
		void poll();
};

#endif
