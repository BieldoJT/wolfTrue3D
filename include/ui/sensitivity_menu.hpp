#ifndef UI_SENSITIVITY_MENU_HPP
# define UI_SENSITIVITY_MENU_HPP

class Input;
class Window;

// Menu temporário (Ctrl) para ajustar a sensibilidade do mouse.
// Desenhado só com scissor + glClear, sem shader nem fonte externa.
class SensitivityMenu
{
	private:
		bool dragging;

		void rect(int x, int y, int w, int h, float r, float g, float b, int winH);
		void text(const char* str, int x, int y, int scale, int winH);

	public:
		static const float MIN_VALUE;
		static const float MAX_VALUE;

		SensitivityMenu();
		void update(const Input& input, Window& window, float& sensitivity);
		void draw(Window& window, float sensitivity);
};

#endif
