#include "ui/hud.hpp"
#include "ui/pixel_text.hpp"
#include "game/components/player_stats.hpp"
#include "game/game_events.hpp"
#include "platform/window.hpp"

#include <glad/glad.h>
#include <cstdio>

namespace
{
	int scaleFor(int h)
	{
		int scale = h / 200;
		return scale < 2 ? 2 : scale;
	}

	// Ícone da chave em retângulos (mesmo desenho do sprite, simplificado).
	void keyIcon(int x, int y, int s, int winH)
	{
		const float r = 0.95f, g = 0.75f, b = 0.2f;
		PixelText::rect(x, y, 5 * s, 5 * s, r, g, b, winH);
		PixelText::rect(x + 2 * s, y + 2 * s, s, s, 0.08f, 0.08f, 0.1f, winH);
		PixelText::rect(x + 5 * s, y + 2 * s, 7 * s, s, r, g, b, winH);
		PixelText::rect(x + 9 * s, y + 3 * s, s, 2 * s, r, g, b, winH);
		PixelText::rect(x + 11 * s, y + 3 * s, s, 2 * s, r, g, b, winH);
	}
}

void Hud::draw(Window& window, const PlayerStats& stats, const GameEvents& events)
{
	int w = window.width();
	int h = window.height();
	int s = scaleFor(h);

	glEnable(GL_SCISSOR_TEST);

	// mira
	PixelText::rect(w / 2 - s / 2, h / 2 - 3 * s, s, 6 * s, 1.0f, 1.0f, 1.0f, h);
	PixelText::rect(w / 2 - 3 * s, h / 2 - s / 2, 6 * s, s, 1.0f, 1.0f, 1.0f, h);

	// barra inferior
	int barH = 13 * s;
	int barY = h - barH;
	PixelText::rect(0, barY, w, barH, 0.10f, 0.11f, 0.14f, h);
	PixelText::rect(0, barY, w, s, 0.35f, 0.37f, 0.42f, h);
	int textY = barY + 4 * s;

	char buf[32];
	std::snprintf(buf, sizeof(buf), "VIDA %d", stats.health);
	bool lowHealth = stats.health <= 25;
	PixelText::text(buf, 4 * s, textY, s, 1.0f, lowHealth ? 0.3f : 1.0f, lowHealth ? 0.3f : 1.0f, h);

	std::snprintf(buf, sizeof(buf), "MUNICAO %d", stats.ammo);
	PixelText::centered(buf, textY, s, 1.0f, 1.0f, 1.0f, w, h);

	if (stats.hasKey)
	{
		int keyX = w - 4 * s - PixelText::textWidth("CHAVE", s) - 15 * s;
		keyIcon(keyX, textY + s, s, h);
		PixelText::text("CHAVE", keyX + 15 * s, textY, s, 0.95f, 0.75f, 0.2f, h);
	}

	if (events.messageTimer > 0.0f && !events.message.empty())
	{
		int msgW = PixelText::textWidth(events.message.c_str(), s);
		int msgY = barY - 16 * s;
		PixelText::rect((w - msgW) / 2 - 3 * s, msgY - 3 * s, msgW + 6 * s, 13 * s, 0.0f, 0.0f, 0.0f, h);
		PixelText::centered(events.message.c_str(), msgY, s, 1.0f, 0.9f, 0.4f, w, h);
	}

	glDisable(GL_SCISSOR_TEST);
}

void Hud::drawGameOver(Window& window)
{
	int w = window.width();
	int h = window.height();
	int s = scaleFor(h);

	glEnable(GL_SCISSOR_TEST);
	PixelText::rect(0, 0, w, h, 0.04f, 0.04f, 0.06f, h);
	PixelText::centered("FIM DE JOGO", h / 2 - 20 * s, s * 3, 0.95f, 0.75f, 0.2f, w, h);
	PixelText::centered("VOCE CHEGOU A SAIDA!", h / 2 + 6 * s, s, 1.0f, 1.0f, 1.0f, w, h);
	PixelText::centered("ENTER RECOMECA    ESC SAI", h / 2 + 20 * s, s, 0.7f, 0.7f, 0.75f, w, h);
	glDisable(GL_SCISSOR_TEST);
}
