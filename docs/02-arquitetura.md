# 02 — Arquitetura

## Visão geral

Executáveis (alvo):

- `wolf` — o jogo.
- `wolf-editor` — o editor de mapas (separado, ver [04-editor.md](04-editor.md)).

Ambos compartilham uma **biblioteca de formato de mapa** (parser/serializador + catálogos), para nunca divergirem.

## Camadas atuais (`include/` e `src/`)

| Pasta | Responsabilidade |
|---|---|
| `app/` | Ciclo de vida: init, loop principal, encerramento |
| `platform/` | Janela SDL2 e entrada (teclado/mouse) |
| `render/` | `Shader`, `Mesh`, `Texture` — wrappers de OpenGL |
| `game/` | `Scene`, `Entity`, componentes, `MapGrid` |
| `game/components/` | `Transform`, `Camera`, `MeshRenderer`, `Collider` |
| `game/systems/` | `CameraSystem`, `MapBuildSystem`, `RenderSystem` |

## Modelo de entidades

`Entity` é uma struct com flags `hasX` + componentes inline (não é um ECS genérico). `Scene` guarda um `std::vector<Entity>`, a malha de cubo compartilhada e o `MapGrid`.

Fluxo atual:

1. `Scene` carrega o mapa (`MapGrid`: `rows` de texto + `cellSize`).
2. `MapBuildSystem` percorre a grade e cria uma entidade-cubo por parede.
3. `CameraSystem` lê a entrada, move o jogador e testa `MapGrid::overlapsWall`.
4. `RenderSystem` desenha as entidades com `MeshRenderer`.

## Decisões de arquitetura

- **Manter o `Entity` simples** enquanto o número de tipos for pequeno. Ao adicionar componentes (`Health`, `Weapon`, `Enemy`, `Door`, `Pickup`), reavaliar migrar para um ECS por tipo **[ABERTO]** — decidir na fase de combate.
- **Mundo = grade + entidades**: a grade define paredes; objetos (inimigos, itens, portas) vêm de uma lista de entidades no arquivo de mapa.
- **Dados antes de código**: texturas, tipos de inimigo e itens vêm de catálogos em `data/`.
- **Render de paredes**: hoje um cubo por célula. Se virar gargalo, fundir faces visíveis numa malha só por mapa **[ABERTO]**.
- **Sprites**: inimigos e itens no estilo Wolfenstein usam billboards (quad voltado à câmera) — decisão a confirmar na fase de inimigos **[ABERTO]**.

## Dependências

C++14, SDL2 (sistema, via `make deps`), glad e stb_image (em `include/`/`src/`), GLM (em `third_party/`).
