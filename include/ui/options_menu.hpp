#ifndef UI_OPTIONS_MENU_HPP
# define UI_OPTIONS_MENU_HPP

# include "platform/window.hpp"

# include <string>
# include <vector>

class Input;
struct Settings;

// Menu de opções (Ctrl): sensibilidade do mouse, resolução e modo de tela.
// Sensibilidade vale na hora; resolução e modo só ao APLICAR.
// Desenhado com PixelText (scissor + glClear), sem shader nem fonte externa.
class OptionsMenu
{
	public:
		enum Action
		{
			None,
			Save,		// algo mudou: gravar settings.txt
			Close		// VOLTAR: fechar o menu (e gravar)
		};

		static const float MIN_SENSITIVITY;
		static const float MAX_SENSITIVITY;

		OptionsMenu();
		// Chame ao abrir o menu: copia as opções atuais para as pendentes.
		void open(const Window& window, const Settings& settings);
		Action update(const Input& input, Window& window, Settings& settings);
		void draw(Window& window, const Settings& settings);

	private:
		enum Row
		{
			RowSensitivity,
			RowResolution,
			RowMode,
			RowButtons,
			RowCount
		};

		int selectedRow;
		int selectedButton;		// 0 APLICAR, 1 VOLTAR
		bool dragging;
		std::vector<Resolution> resolutions;
		int resolutionIndex;
		DisplayMode pendingMode;
		std::string status;

		void changeResolution(int delta);
		void changeMode(int delta);
		bool apply(Window& window, Settings& settings);
};

#endif
