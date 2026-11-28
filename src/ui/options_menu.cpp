#include "ui/options_menu.hpp"
#include "ui/pixel_text.hpp"
#include "app/settings.hpp"
#include "platform/input.hpp"

#include <glad/glad.h>
#include <algorithm>
#include <cstdio>

const float OptionsMenu::MIN_SENSITIVITY = 0.01f;
const float OptionsMenu::MAX_SENSITIVITY = 0.30f;

namespace
{
	const float KEY_STEP = 0.005f;

	struct Box
	{
		int x, y, w, h;

		bool contains(int px, int py) const
		{
			return px >= x && px < x + w && py >= y && py < y + h;
		}
	};

	// Medidas em unidades de "s" (tamanho de um pixel da fonte), recalculadas a cada quadro.
	struct Layout
	{
		int s;
		Box panel;
		int rowY[4];
		int labelX;
		int valueX;
		int valueW;
		Box track;
		Box resLeft, resRight, modeLeft, modeRight;
		Box applyButton, backButton;
		int statusY, hintY;
	};

	Layout layoutFor(int w, int h)
	{
		Layout l;
		l.s = std::max(1, std::min(h / 200, w / 245));
		int s = l.s;
		l.panel.w = 224 * s;
		l.panel.h = 132 * s;
		l.panel.x = (w - l.panel.w) / 2;
		l.panel.y = (h - l.panel.h) / 2;
		for (int i = 0; i < 3; ++i)
			l.rowY[i] = l.panel.y + 30 * s + i * 16 * s;
		l.rowY[3] = l.panel.y + 84 * s;
		l.labelX = l.panel.x + 8 * s;
		l.valueX = l.panel.x + 92 * s;
		l.valueW = 124 * s;

		Box track = {l.valueX, l.rowY[0] + 2 * s, 70 * s, 3 * s};
		l.track = track;
		Box resLeft = {l.valueX - 2 * s, l.rowY[1] - 2 * s, 9 * s, 11 * s};
		Box resRight = {l.valueX + l.valueW - 7 * s, l.rowY[1] - 2 * s, 9 * s, 11 * s};
		l.resLeft = resLeft;
		l.resRight = resRight;
		l.modeLeft = resLeft;
		l.modeLeft.y = l.rowY[2] - 2 * s;
		l.modeRight = resRight;
		l.modeRight.y = l.rowY[2] - 2 * s;

		int applyW = PixelText::textWidth("APLICAR", s) + 10 * s;
		int backW = PixelText::textWidth("VOLTAR", s) + 10 * s;
		int gap = 12 * s;
		int startX = l.panel.x + (l.panel.w - applyW - gap - backW) / 2;
		Box applyButton = {startX, l.rowY[3], applyW, 13 * s};
		Box backButton = {startX + applyW + gap, l.rowY[3], backW, 13 * s};
		l.applyButton = applyButton;
		l.backButton = backButton;
		l.statusY = l.rowY[3] + 19 * s;
		l.hintY = l.statusY + 12 * s;
		return l;
	}

	float clampSensitivity(float v)
	{
		return std::max(OptionsMenu::MIN_SENSITIVITY, std::min(OptionsMenu::MAX_SENSITIVITY, v));
	}

	const char* modeLabel(DisplayMode mode)
	{
		if (mode == DisplayBorderless)
			return "JANELA SEM BORDA";
		if (mode == DisplayFullscreen)
			return "TELA CHEIA";
		return "JANELA";
	}

	void box(const Box& b, float r, float g, float bl, int winH)
	{
		PixelText::rect(b.x, b.y, b.w, b.h, r, g, bl, winH);
	}

	void centeredIn(const char* text, int x, int w, int y, int s, float r, float g, float b, int winH)
	{
		PixelText::text(text, x + (w - PixelText::textWidth(text, s)) / 2, y, s, r, g, b, winH);
	}
}

OptionsMenu::OptionsMenu()
	: selectedRow(RowSensitivity), selectedButton(0), dragging(false),
	  resolutionIndex(0), pendingMode(DisplayWindowed)
{}

void OptionsMenu::open(const Window& window, const Settings& settings)
{
	this->resolutions = window.availableResolutions();
	if (std::find(this->resolutions.begin(), this->resolutions.end(), settings.resolution) == this->resolutions.end())
		this->resolutions.push_back(settings.resolution);
	this->resolutionIndex = static_cast<int>(
		std::find(this->resolutions.begin(), this->resolutions.end(), settings.resolution) - this->resolutions.begin());
	this->pendingMode = settings.mode;
	this->status.clear();
	this->dragging = false;
	this->selectedRow = RowSensitivity;
	this->selectedButton = 0;
}

void OptionsMenu::changeResolution(int delta)
{
	if (this->pendingMode == DisplayBorderless || this->resolutions.empty())
		return;
	int count = static_cast<int>(this->resolutions.size());
	this->resolutionIndex = (this->resolutionIndex + delta + count) % count;
	this->status.clear();
}

void OptionsMenu::changeMode(int delta)
{
	this->pendingMode = static_cast<DisplayMode>((this->pendingMode + delta + 3) % 3);
	this->status.clear();
}

bool OptionsMenu::apply(Window& window, Settings& settings)
{
	Resolution size = this->resolutions.empty() ? settings.resolution
		: this->resolutions[static_cast<std::size_t>(this->resolutionIndex)];
	if (!window.applyVideo(size, this->pendingMode))
	{
		this->status = "MODO NAO SUPORTADO";
		this->pendingMode = settings.mode;
		return false;
	}
	settings.resolution = size;
	settings.mode = this->pendingMode;
	this->status = "APLICADO";
	return true;
}

OptionsMenu::Action OptionsMenu::update(const Input& input, Window& window, Settings& settings)
{
	if (input.mouseCaptured)
	{
		this->dragging = false;
		return None;
	}

	Layout l = layoutFor(window.width(), window.height());
	// Mouse vem em coordenadas de janela; o desenho é em pixels.
	int mx = input.mouseX * window.width() / std::max(1, window.windowWidth());
	int my = input.mouseY * window.height() / std::max(1, window.windowHeight());
	Action action = None;
	float before = settings.mouseSensitivity;

	// Teclado
	if (input.menuUp)
		this->selectedRow = (this->selectedRow + RowCount - 1) % RowCount;
	if (input.menuDown)
		this->selectedRow = (this->selectedRow + 1) % RowCount;
	int step = (input.menuRight ? 1 : 0) - (input.menuLeft ? 1 : 0);
	if (step != 0)
	{
		if (this->selectedRow == RowSensitivity)
			settings.mouseSensitivity += step * KEY_STEP;
		else if (this->selectedRow == RowResolution)
			this->changeResolution(step);
		else if (this->selectedRow == RowMode)
			this->changeMode(step);
		else
			this->selectedButton = step > 0 ? 1 : 0;
	}
	if (input.confirm)
	{
		if (this->selectedRow == RowButtons && this->selectedButton == 1)
			return Close;
		if (this->apply(window, settings))
			action = Save;
	}

	// Mouse: slider de sensibilidade
	int slop = l.track.h * 3;
	bool overTrack = mx >= l.track.x - slop && mx <= l.track.x + l.track.w + slop
		&& my >= l.track.y - slop && my <= l.track.y + l.track.h + slop;
	if (!input.mouseDown)
		this->dragging = false;
	else if (input.mouseClicked && overTrack)
		this->dragging = true;
	if (this->dragging)
	{
		this->selectedRow = RowSensitivity;
		float t = static_cast<float>(mx - l.track.x) / static_cast<float>(l.track.w);
		t = std::max(0.0f, std::min(1.0f, t));
		settings.mouseSensitivity = MIN_SENSITIVITY + t * (MAX_SENSITIVITY - MIN_SENSITIVITY);
	}

	// Mouse: setas e botões
	if (input.mouseClicked && !this->dragging)
	{
		if (l.resLeft.contains(mx, my) || l.resRight.contains(mx, my))
		{
			this->selectedRow = RowResolution;
			this->changeResolution(l.resLeft.contains(mx, my) ? -1 : 1);
		}
		else if (l.modeLeft.contains(mx, my) || l.modeRight.contains(mx, my))
		{
			this->selectedRow = RowMode;
			this->changeMode(l.modeLeft.contains(mx, my) ? -1 : 1);
		}
		else if (l.applyButton.contains(mx, my))
		{
			this->selectedRow = RowButtons;
			this->selectedButton = 0;
			if (this->apply(window, settings))
				action = Save;
		}
		else if (l.backButton.contains(mx, my))
			return Close;
	}

	settings.mouseSensitivity = clampSensitivity(settings.mouseSensitivity);
	if (action == None && settings.mouseSensitivity != before && !this->dragging)
		action = Save;
	return action;
}

void OptionsMenu::draw(Window& window, const Settings& settings)
{
	int w = window.width();
	int h = window.height();
	Layout l = layoutFor(w, h);
	int s = l.s;
	const float hi[3] = {1.0f, 0.8f, 0.25f};	// amarelo: pendente/selecionado

	glEnable(GL_SCISSOR_TEST);
	box(l.panel, 0.05f, 0.05f, 0.08f, h);
	Box border = {l.panel.x, l.panel.y, l.panel.w, s};
	box(border, 0.35f, 0.37f, 0.42f, h);

	centeredIn("OPCOES", l.panel.x, l.panel.w, l.panel.y + 7 * s, 2 * s, 1.0f, 1.0f, 1.0f, h);

	// Linha selecionada
	if (this->selectedRow != RowButtons)
	{
		Box sel = {l.panel.x + 3 * s, l.rowY[this->selectedRow] - 4 * s, l.panel.w - 6 * s, 15 * s};
		box(sel, 0.13f, 0.14f, 0.20f, h);
	}

	const char* labels[3] = {"SENSIBILIDADE", "RESOLUCAO", "MODO"};
	for (int i = 0; i < 3; ++i)
		PixelText::text(labels[i], l.labelX, l.rowY[i], s, 0.8f, 0.8f, 0.85f, h);

	// Sensibilidade
	box(l.track, 0.3f, 0.3f, 0.35f, h);
	float t = (settings.mouseSensitivity - MIN_SENSITIVITY) / (MAX_SENSITIVITY - MIN_SENSITIVITY);
	int fill = static_cast<int>(t * l.track.w);
	PixelText::rect(l.track.x, l.track.y, fill, l.track.h, 0.9f, 0.6f, 0.1f, h);
	PixelText::rect(l.track.x + fill - s, l.track.y - 2 * s, 2 * s, l.track.h + 4 * s, 1.0f, 1.0f, 1.0f, h);
	char buf[32];
	std::snprintf(buf, sizeof(buf), "%.3f", settings.mouseSensitivity);
	PixelText::text(buf, l.track.x + l.track.w + 8 * s, l.rowY[0], s, 1.0f, 1.0f, 1.0f, h);

	// Resolução
	bool borderless = this->pendingMode == DisplayBorderless;
	Resolution res = this->resolutions.empty() ? settings.resolution
		: this->resolutions[static_cast<std::size_t>(this->resolutionIndex)];
	if (borderless)
		res = window.desktopResolution();
	std::snprintf(buf, sizeof(buf), "%dX%d", res.width, res.height);
	bool resPending = !borderless && !(res == settings.resolution);
	float rr = borderless ? 0.45f : (resPending ? hi[0] : 1.0f);
	float rg = borderless ? 0.45f : (resPending ? hi[1] : 1.0f);
	float rb = borderless ? 0.5f : (resPending ? hi[2] : 1.0f);
	centeredIn(buf, l.valueX, l.valueW, l.rowY[1], s, rr, rg, rb, h);
	if (!borderless)
	{
		PixelText::text("<", l.resLeft.x + 2 * s, l.rowY[1], s, 1.0f, 1.0f, 1.0f, h);
		PixelText::text(">", l.resRight.x + 2 * s, l.rowY[1], s, 1.0f, 1.0f, 1.0f, h);
	}

	// Modo
	bool modePending = this->pendingMode != settings.mode;
	centeredIn(modeLabel(this->pendingMode), l.valueX, l.valueW, l.rowY[2], s,
		modePending ? hi[0] : 1.0f, modePending ? hi[1] : 1.0f, modePending ? hi[2] : 1.0f, h);
	PixelText::text("<", l.modeLeft.x + 2 * s, l.rowY[2], s, 1.0f, 1.0f, 1.0f, h);
	PixelText::text(">", l.modeRight.x + 2 * s, l.rowY[2], s, 1.0f, 1.0f, 1.0f, h);

	// Botões
	bool onButtons = this->selectedRow == RowButtons;
	bool pending = resPending || modePending;
	box(l.applyButton, pending ? 0.55f : 0.25f, pending ? 0.4f : 0.27f, pending ? 0.08f : 0.32f, h);
	box(l.backButton, 0.25f, 0.27f, 0.32f, h);
	if (onButtons)
	{
		const Box& b = this->selectedButton == 0 ? l.applyButton : l.backButton;
		Box line = {b.x, b.y + b.h - s, b.w, s};
		box(line, 1.0f, 1.0f, 1.0f, h);
	}
	centeredIn("APLICAR", l.applyButton.x, l.applyButton.w, l.applyButton.y + 3 * s, s, 1.0f, 1.0f, 1.0f, h);
	centeredIn("VOLTAR", l.backButton.x, l.backButton.w, l.backButton.y + 3 * s, s, 1.0f, 1.0f, 1.0f, h);

	if (!this->status.empty())
	{
		bool ok = this->status == "APLICADO";
		centeredIn(this->status.c_str(), l.panel.x, l.panel.w, l.statusY, s,
			ok ? 0.4f : 1.0f, ok ? 1.0f : 0.35f, ok ? 0.5f : 0.35f, h);
	}
	centeredIn("ENTER APLICA   CTRL VOLTA", l.panel.x, l.panel.w, l.hintY, s,
		0.55f, 0.55f, 0.6f, h);
	glDisable(GL_SCISSOR_TEST);
}
