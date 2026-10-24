#include "ui/sensitivity_menu.hpp"
#include "game/map_grid.hpp"
#include "platform/input.hpp"
#include "platform/window.hpp"

#include <glad/glad.h>
#include <cstdio>
#include <cstring>

const float SensitivityMenu::MIN_VALUE = 0.01f;
const float SensitivityMenu::MAX_VALUE = 0.30f;

namespace
{
	const float KEY_STEP = 0.005f;

	struct Glyph
	{
		char c;
		const char* rows[7];
	};

	const Glyph FONT[] = {
		{'A', {".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
		{'B', {"####.", "#...#", "#...#", "####.", "#...#", "#...#", "####."}},
		{'C', {".####", "#....", "#....", "#....", "#....", "#....", ".####"}},
		{'D', {"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."}},
		{'E', {"#####", "#....", "#....", "####.", "#....", "#....", "#####"}},
		{'I', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "#####"}},
		{'L', {"#....", "#....", "#....", "#....", "#....", "#....", "#####"}},
		{'M', {"#...#", "##.##", "#.#.#", "#.#.#", "#...#", "#...#", "#...#"}},
		{'N', {"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"}},
		{'O', {".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
		{'R', {"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"}},
		{'S', {".####", "#....", "#....", ".###.", "....#", "....#", "####."}},
		{'T', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}},
		{'U', {"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
		{'V', {"#...#", "#...#", "#...#", "#...#", "#...#", ".#.#.", "..#.."}},
		{'0', {".###.", "#...#", "#..##", "#.#.#", "##..#", "#...#", ".###."}},
		{'1', {"..#..", ".##..", "..#..", "..#..", "..#..", "..#..", ".###."}},
		{'2', {".###.", "#...#", "....#", "...#.", "..#..", ".#...", "#####"}},
		{'3', {"####.", "....#", "....#", ".###.", "....#", "....#", "####."}},
		{'4', {"...#.", "..##.", ".#.#.", "#..#.", "#####", "...#.", "...#."}},
		{'5', {"#####", "#....", "####.", "....#", "....#", "#...#", ".###."}},
		{'6', {".###.", "#....", "#....", "####.", "#...#", "#...#", ".###."}},
		{'7', {"#####", "....#", "...#.", "..#..", ".#...", ".#...", ".#..."}},
		{'8', {".###.", "#...#", "#...#", ".###.", "#...#", "#...#", ".###."}},
		{'9', {".###.", "#...#", "#...#", ".####", "....#", "....#", ".###."}},
		{'.', {".....", ".....", ".....", ".....", ".....", ".##..", ".##.."}},
	};

	const Glyph* findGlyph(char c)
	{
		for (size_t i = 0; i < sizeof(FONT) / sizeof(FONT[0]); ++i)
			if (FONT[i].c == c)
				return &FONT[i];
		return 0;
	}

	float clampValue(float v)
	{
		if (v < SensitivityMenu::MIN_VALUE)
			return SensitivityMenu::MIN_VALUE;
		if (v > SensitivityMenu::MAX_VALUE)
			return SensitivityMenu::MAX_VALUE;
		return v;
	}

	struct Layout
	{
		int trackX, trackY, trackW, trackH;
	};

	Layout layoutFor(int w, int h)
	{
		Layout l;
		l.trackW = w / 3;
		l.trackH = h / 60 + 4;
		l.trackX = (w - l.trackW) / 2;
		l.trackY = h / 2;
		return l;
	}
}

SensitivityMenu::SensitivityMenu()
	: dragging(false)
{}

void SensitivityMenu::update(const Input& input, Window& window, float& sensitivity)
{
	if (input.mouseCaptured)
	{
		this->dragging = false;
		return;
	}

	Layout l = layoutFor(window.width(), window.height());
	int slop = l.trackH * 2;
	bool overTrack = input.mouseX >= l.trackX - slop
		&& input.mouseX <= l.trackX + l.trackW + slop
		&& input.mouseY >= l.trackY - slop
		&& input.mouseY <= l.trackY + l.trackH + slop;

	if (!input.mouseDown)
		this->dragging = false;
	else if (overTrack)
		this->dragging = true;

	if (this->dragging)
	{
		float t = static_cast<float>(input.mouseX - l.trackX) / static_cast<float>(l.trackW);
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;
		sensitivity = MIN_VALUE + t * (MAX_VALUE - MIN_VALUE);
	}
	if (input.menuLeft)
		sensitivity -= KEY_STEP;
	if (input.menuRight)
		sensitivity += KEY_STEP;
	sensitivity = clampValue(sensitivity);
}

void SensitivityMenu::rect(int x, int y, int w, int h, float r, float g, float b, int winH)
{
	glScissor(x, winH - y - h, w, h);
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void SensitivityMenu::text(const char* str, int x, int y, int scale, int winH)
{
	for (size_t i = 0; i < std::strlen(str); ++i)
	{
		const Glyph* g = findGlyph(str[i]);
		if (g)
			for (int row = 0; row < 7; ++row)
				for (int col = 0; col < 5; ++col)
					if (MapGrid::isWallCell(g->rows[row][col]))
						this->rect(x + col * scale, y + row * scale, scale, scale,
							1.0f, 1.0f, 1.0f, winH);
		x += 6 * scale;
	}
}

void SensitivityMenu::draw(Window& window, float sensitivity)
{
	int w = window.width();
	int h = window.height();
	Layout l = layoutFor(w, h);
	int scale = h / 180;
	if (scale < 2)
		scale = 2;

	glEnable(GL_SCISSOR_TEST);

	int panelW = l.trackW + l.trackW / 2;
	int panelH = 22 * scale + l.trackH + 8 * scale;
	int panelX = (w - panelW) / 2;
	int panelY = l.trackY - 12 * scale;
	this->rect(panelX, panelY, panelW, panelH, 0.05f, 0.05f, 0.08f, h);

	const char* title = "SENSIBILIDADE DO MOUSE";
	int titleW = static_cast<int>(std::strlen(title)) * 6 * scale;
	this->text(title, (w - titleW) / 2, panelY + 2 * scale, scale, h);

	this->rect(l.trackX, l.trackY, l.trackW, l.trackH, 0.3f, 0.3f, 0.35f, h);
	float t = (sensitivity - MIN_VALUE) / (MAX_VALUE - MIN_VALUE);
	int fill = static_cast<int>(t * l.trackW);
	this->rect(l.trackX, l.trackY, fill, l.trackH, 0.9f, 0.6f, 0.1f, h);
	this->rect(l.trackX + fill - scale, l.trackY - scale * 2, scale * 2, l.trackH + scale * 4,
		1.0f, 1.0f, 1.0f, h);

	char buf[16];
	std::snprintf(buf, sizeof(buf), "%.3f", sensitivity);
	int valueW = static_cast<int>(std::strlen(buf)) * 6 * scale;
	this->text(buf, (w - valueW) / 2, l.trackY + l.trackH + 3 * scale, scale, h);

	const char* hint = "CTRL VOLTAR";
	int hintW = static_cast<int>(std::strlen(hint)) * 6 * scale;
	this->text(hint, (w - hintW) / 2, l.trackY + l.trackH + 11 * scale, scale, h);

	glDisable(GL_SCISSOR_TEST);
}
