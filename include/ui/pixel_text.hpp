#ifndef UI_PIXEL_TEXT_HPP
# define UI_PIXEL_TEXT_HPP

// Desenho 2D mínimo com scissor + glClear (sem shader nem fonte externa), usado pelo HUD e menus.
// Coordenadas em pixels a partir do canto superior esquerdo da janela.
// Chame com GL_SCISSOR_TEST ligado.
namespace PixelText
{
	// Fonte 5x7: A-Z, 0-9, espaço e + - : . ! % < >
	void rect(int x, int y, int w, int h, float r, float g, float b, int winH);
	void text(const char* str, int x, int y, int scale, float r, float g, float b, int winH);
	int textWidth(const char* str, int scale);
	// Texto centralizado horizontalmente na janela.
	void centered(const char* str, int y, int scale, float r, float g, float b, int winW, int winH);
}

#endif
