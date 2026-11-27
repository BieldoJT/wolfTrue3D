# 05 — Assets

Princípio: **adicionar conteúdo = colocar um arquivo + uma linha num catálogo**. Nada de recompilar.

## Pastas

| Pasta | Conteúdo |
|---|---|
| `images/` | Texturas e sprites (PNG, carregados com stb_image) |
| `data/` | Catálogos de assets |
| `maps/` | Mapas |
| `shaders/` | GLSL |

## Catálogo de texturas (existente)

`data/textures.txt`: uma linha por textura, `id caminho`.

```
1 images/CRVascodaGama png.png
4 images/porta.png
```

Mapas referenciam o **id**, nunca o caminho. Ids são estáveis: não renumerar.

## Catálogo de itens (existente)

`data/items.txt`: uma linha por item coletável, `símbolo nome textura efeito quantidade`.

```
A municao 7 ammo 8
H vida 8 health 25
K chave 9 key 1
```

- **símbolo**: letra maiúscula usada na grade do mapa (exceto `P`, `D`, `L`, `X`, reservadas).
- **textura**: id do sprite em `data/textures.txt`. Sprites são PNG com fundo transparente (pixels com alfa < 50% não desenham) e aparecem como billboard, sempre de frente para a câmera, apoiados no chão.
- **efeito**: `ammo` (munição, máx. 99), `health` (vida, máx. 100) ou `key` (abre as portas `L` do mapa). Com vida ou munição cheias o item fica no chão.
- Item novo = sprite + linha no `textures.txt` + linha aqui, sem recompilar. Efeitos novos exigem código.

## Assets da Fase 2

Portas, interruptor de saída e sprites de itens (ids 4–9) são pixel art 64×64 **gerada por código**: `python3 scripts/gerar-assets.py` (Python puro, sem dependências) recria os PNGs em `images/`. Para trocar o visual, edite o script ou substitua os PNGs mantendo os ids.

## Catálogos a criar (conforme as fases)

| Arquivo | Conteúdo | Fase |
|---|---|---|
| `data/enemies.txt` | tipo, sprite, vida, velocidade, dano | Combate |
| `data/weapons.txt` | nome, sprite, dano, cadência, munição | Combate |

Formato sugerido: mesma ideia do atual, linhas de texto simples **[ABERTO — definir campos na fase correspondente]**.

## Regras

- Texturas quadradas e potência de 2 (64×64 ou 128×128) para estilo e desempenho.
- Nomes de arquivo sem espaços em assets novos (o atual `CRVascodaGama png.png` é legado).
- Respeitar licença: só adicionar assets próprios ou com licença livre; anotar a origem em `images/CREDITS.md`.

## Futuro

Importar texturas do próprio jogador pela UI do editor, e depois sons e modelos. Fora da v1.
