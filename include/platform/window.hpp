#ifndef PLATFORM_WINDOW_HPP
# define PLATFORM_WINDOW_HPP

# include <SDL.h>

class Window
{
	private:
		SDL_Window* window;
		SDL_GLContext glContext;
		bool sdlReady;

	public:
		Window();
		~Window();

		void swap();
		int width() const;
		int height() const;
};

#endif
