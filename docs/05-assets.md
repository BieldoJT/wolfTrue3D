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
```

Mapas referenciam o **id**, nunca o caminho. Ids são estáveis: não renumerar.

## Catálogos a criar (conforme as fases)

| Arquivo | Conteúdo | Fase |
|---|---|---|
| `data/enemies.txt` | tipo, sprite, vida, velocidade, dano | Combate |
| `data/items.txt` | tipo, sprite, efeito (munição, vida, chave) | Combate |
| `data/weapons.txt` | nome, sprite, dano, cadência, munição | Combate |

Formato sugerido: mesma ideia do atual, linhas de texto simples **[ABERTO — definir campos na fase correspondente]**.

## Regras

- Texturas quadradas e potência de 2 (64×64 ou 128×128) para estilo e desempenho.
- Nomes de arquivo sem espaços em assets novos (o atual `CRVascodaGama png.png` é legado).
- Respeitar licença: só adicionar assets próprios ou com licença livre; anotar a origem num `images/CREDITS.md` quando houver terceiros.

## Futuro

Importar texturas do próprio jogador pela UI do editor, e depois sons e modelos. Fora da v1.
