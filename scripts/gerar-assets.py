#!/usr/bin/env python3
"""Gera as texturas e sprites da Fase 2 (portas, saída e itens) em pixel art 64x64.

Python puro (zlib + struct), sem dependências. Rode da raiz do projeto:

    python3 scripts/gerar-assets.py

Sobrescreve os arquivos em images/. Os assets são próprios do projeto (ver images/CREDITS.md).
"""
import os
import random
import struct
import zlib

SIZE = 64
OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "images")


class Image:
    def __init__(self, fill=(0, 0, 0, 0)):
        self.px = [[fill for _ in range(SIZE)] for _ in range(SIZE)]

    def set(self, x, y, color):
        if 0 <= x < SIZE and 0 <= y < SIZE:
            if len(color) == 3:
                color = (color[0], color[1], color[2], 255)
            self.px[y][x] = color

    def get(self, x, y):
        return self.px[y][x]

    def rect(self, x0, y0, x1, y1, color):
        """Retângulo preenchido, limites inclusivos."""
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                self.set(x, y, color)

    def frame(self, x0, y0, x1, y1, light, dark):
        """Borda em relevo: claro em cima/esquerda, escuro em baixo/direita."""
        for x in range(x0, x1 + 1):
            self.set(x, y0, light)
            self.set(x, y1, dark)
        for y in range(y0, y1 + 1):
            self.set(x0, y, light)
            self.set(x1, y, dark)

    def circle(self, cx, cy, r, color):
        for y in range(cy - r, cy + r + 1):
            for x in range(cx - r, cx + r + 1):
                if (x - cx) ** 2 + (y - cy) ** 2 <= r * r + r * 0.6:
                    self.set(x, y, color)

    def noise(self, x0, y0, x1, y1, amount, rng):
        """Variação leve de brilho para não ficar chapado."""
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                r, g, b, a = self.get(x, y)
                if a == 0:
                    continue
                d = rng.randint(-amount, amount)
                self.set(x, y, (clamp(r + d), clamp(g + d), clamp(b + d), a))

    def save(self, name, alpha):
        path = os.path.normpath(os.path.join(OUT_DIR, name))
        rows = b""
        for row in self.px:
            rows += b"\x00"
            for r, g, b, a in row:
                rows += bytes((r, g, b, a)) if alpha else bytes((r, g, b))
        color_type = 6 if alpha else 2

        def chunk(tag, data):
            body = tag + data
            return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)

        png = b"\x89PNG\r\n\x1a\n"
        png += chunk(b"IHDR", struct.pack(">IIBBBBB", SIZE, SIZE, 8, color_type, 0, 0, 0))
        png += chunk(b"IDAT", zlib.compress(rows, 9))
        png += chunk(b"IEND", b"")
        with open(path, "wb") as f:
            f.write(png)
        print("gerado", path)


def clamp(v):
    return max(0, min(255, v))


def shade(color, d):
    return tuple(clamp(c + d) for c in color[:3])


# Fonte 3x5 para letreiros nas texturas.
FONT3 = {
    "S": ["###", "#..", "###", "..#", "###"],
    "A": ["###", "#.#", "###", "#.#", "#.#"],
    "I": ["###", ".#.", ".#.", ".#.", "###"],
    "D": ["##.", "#.#", "#.#", "#.#", "##."],
}


def text3(img, s, x, y, color, scale=1):
    for ch in s:
        glyph = FONT3[ch]
        for gy, line in enumerate(glyph):
            for gx, c in enumerate(line):
                if c == "#":
                    img.rect(x + gx * scale, y + gy * scale, x + gx * scale + scale - 1, y + gy * scale + scale - 1, color)
        x += 4 * scale


def door_base(base, rng):
    """Porta de metal com dois painéis, rebites e puxador."""
    img = Image()
    img.rect(0, 0, 63, 63, base)
    img.noise(0, 0, 63, 63, 6, rng)
    img.frame(0, 0, 63, 63, shade(base, 40), shade(base, -60))
    img.frame(1, 1, 62, 62, shade(base, 25), shade(base, -40))
    # painéis rebaixados
    for (x0, x1) in ((6, 29), (34, 57)):
        img.frame(x0, 6, x1, 57, shade(base, -45), shade(base, 30))
        for y in range(10, 54, 6):
            img.rect(x0 + 3, y, x1 - 3, y, shade(base, -18))
    # rebites
    for x in (3, 31, 32, 60):
        for y in (3, 20, 43, 60):
            img.set(x, y, shade(base, 60))
            img.set(x + 1, y + 1, shade(base, -70))
    return img


def make_door():
    rng = random.Random(1)
    img = door_base((92, 108, 128), rng)
    # puxador vertical no painel direito
    img.rect(37, 26, 39, 38, (40, 40, 44))
    img.rect(37, 26, 37, 38, (150, 150, 160))
    img.save("porta.png", alpha=False)


def make_locked_door():
    rng = random.Random(2)
    img = door_base((120, 92, 52), rng)
    # faixas de alerta amarelas/pretas em cima e embaixo
    for y0 in (8, 52):
        for x in range(7, 57):
            for y in range(y0, y0 + 4):
                img.set(x, y, (230, 190, 40) if ((x + y) // 3) % 2 == 0 else (30, 30, 30))
    # fechadura dourada com buraco de chave
    img.rect(25, 22, 38, 41, (60, 45, 20))
    img.rect(26, 23, 37, 40, (214, 172, 56))
    img.frame(26, 23, 37, 40, (250, 220, 120), (150, 110, 30))
    img.circle(31, 29, 3, (25, 20, 15))
    img.rect(30, 30, 32, 36, (25, 20, 15))
    img.save("porta_trancada.png", alpha=False)


def make_exit():
    """Interruptor de saída: painel metálico, letreiro verde SAIDA e alavanca."""
    rng = random.Random(3)
    base = (88, 88, 92)
    img = Image()
    img.rect(0, 0, 63, 63, base)
    img.noise(0, 0, 63, 63, 8, rng)
    img.frame(0, 0, 63, 63, shade(base, 40), shade(base, -55))
    # letreiro
    img.rect(8, 6, 55, 20, (15, 40, 20))
    img.frame(8, 6, 55, 20, (40, 90, 50), (5, 20, 10))
    text3(img, "SAIDA", 13, 9, (90, 255, 120), scale=2)
    # caixa da alavanca
    img.rect(18, 26, 45, 57, (50, 50, 55))
    img.frame(18, 26, 45, 57, (120, 120, 128), (25, 25, 28))
    img.rect(29, 30, 34, 53, (20, 20, 22))
    # alavanca (para cima) com manopla vermelha
    img.rect(30, 34, 33, 45, (170, 170, 175))
    img.rect(27, 30, 36, 35, (200, 40, 40))
    img.rect(27, 30, 36, 30, (250, 110, 110))
    # luzes
    img.circle(13, 42, 2, (60, 230, 90))
    img.circle(50, 42, 2, (230, 60, 60))
    img.save("saida.png", alpha=False)


def make_ammo():
    """Caixa de munição com balas por cima (fundo transparente)."""
    img = Image()
    box = (96, 110, 60)
    # balas em pé atrás da caixa
    for i, x in enumerate(range(18, 46, 6)):
        img.rect(x, 26, x + 3, 40, (200, 160, 60))
        img.rect(x, 26, x, 40, (240, 210, 120))
        img.rect(x, 22, x + 3, 25, (170, 110, 60))
        img.rect(x + 1, 20, x + 2, 21, (170, 110, 60))
    # caixa
    img.rect(10, 38, 53, 62, box)
    img.frame(10, 38, 53, 62, shade(box, 35), shade(box, -45))
    img.rect(12, 40, 51, 42, shade(box, 20))
    # etiqueta
    img.rect(22, 47, 41, 56, (220, 210, 170))
    img.frame(22, 47, 41, 56, (250, 245, 220), (150, 140, 100))
    for x in range(25, 39, 4):
        img.rect(x, 50, x + 1, 53, (60, 50, 30))
    img.save("municao.png", alpha=True)


def make_health():
    """Kit de primeiros socorros branco com cruz vermelha (fundo transparente)."""
    img = Image()
    white = (225, 228, 230)
    # alça
    img.rect(24, 28, 39, 31, (90, 90, 95))
    img.rect(24, 28, 26, 36, (90, 90, 95))
    img.rect(37, 28, 39, 36, (90, 90, 95))
    # caixa
    img.rect(8, 34, 55, 62, white)
    img.frame(8, 34, 55, 62, (255, 255, 255), (140, 145, 150))
    img.rect(8, 40, 55, 40, (180, 185, 190))
    # cruz
    img.rect(28, 43, 35, 60, (210, 30, 35))
    img.rect(21, 48, 42, 55, (210, 30, 35))
    img.rect(28, 43, 35, 43, (250, 90, 90))
    img.rect(21, 48, 27, 48, (250, 90, 90))
    img.save("vida.png", alpha=True)


def make_key():
    """Chave dourada deitada (fundo transparente)."""
    img = Image()
    gold = (230, 185, 50)
    dark = (150, 105, 20)
    light = (255, 235, 140)
    # argola
    img.circle(18, 48, 10, gold)
    img.circle(18, 48, 5, (0, 0, 0, 0))
    for (x, y) in ((12, 41), (13, 40), (14, 39), (15, 39)):
        img.set(x, y, light)
    # haste
    img.rect(27, 46, 56, 50, gold)
    img.rect(27, 46, 56, 46, light)
    img.rect(27, 50, 56, 50, dark)
    # dentes
    img.rect(46, 51, 49, 57, gold)
    img.rect(52, 51, 55, 55, gold)
    img.rect(49, 51, 49, 57, dark)
    img.rect(55, 51, 55, 55, dark)
    img.save("chave.png", alpha=True)


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    make_door()
    make_locked_door()
    make_exit()
    make_ammo()
    make_health()
    make_key()


if __name__ == "__main__":
    main()
