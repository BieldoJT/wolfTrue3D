# 02 — Arquitetura

## Visão geral

Executáveis (alvo):

- `wolf` — o jogo.
- `wolf-editor` — o editor de mapas (separado, ver [04-editor.md](04-editor.md)).

Ambos compartilham uma **biblioteca de formato de mapa** (parser/serializador + catálogos), para nunca divergirem.

## Camadas atuais (`include/` e `src/`)

| Pasta | Responsabilidade |
|---|---|
| `app/` | Ciclo de vida: init, loop principal, encerramento; `Settings` (opções do jogador em `settings.txt`) |
| `map/` | Biblioteca de formato: parser, validador e catálogo de texturas, sem SDL/GL (compartilhada com `mapcheck` e o editor) |
| `platform/` | Janela SDL2 (resolução e modo: janela, janela sem borda, tela cheia) e entrada (teclado/mouse) |
| `render/` | `Shader`, `Mesh`, `Texture` — wrappers de OpenGL |
| `game/` | `Scene`, `Entity`, componentes, `MapGrid`, `GameEvents` |
| `game/components/` | `Transform`, `Camera`, `MeshRenderer`, `Collider`, `Door`, `Pickup`, `PlayerStats` |
| `game/systems/` | `CameraSystem`, `MapBuildSystem`, `InteractionSystem`, `DoorSystem`, `PickupSystem`, `RenderSystem` |
| `ui/` | `PixelText` (retângulos e fonte 5×7 via scissor), `Hud`, `OptionsMenu` |

## Modelo de entidades

`Entity` é uma struct com flags `hasX` + componentes inline (não é um ECS genérico). `Scene` guarda um `std::vector<Entity>`, as malhas compartilhadas (cubo, plano, quad de sprite), o `MapGrid` e os `GameEvents` (mensagem do HUD e pedido de saída de fase).

Fluxo atual:

1. `App` lê os catálogos de texturas e itens, chama `loadMapFile` + `validateMap` (módulo `map/`) e imprime erros/avisos; com erro, encerra.
2. `MapBuildSystem` recebe o `MapData` validado, preenche o `MapGrid` da `Scene` e cria chão, teto, um cubo por parede/`X`, uma folha fina por porta (`Door`), um sprite por item (`Pickup`) e o jogador (com `PlayerStats`).
3. A cada quadro: `CameraSystem` move o jogador (colisão por `MapGrid::overlapsWall`, que trata porta fechada como parede); `InteractionSystem` lança um raio curto na direção do olhar quando o jogador aperta E; `DoorSystem` anima as portas e libera a passagem no `MapGrid`; `PickupSystem` coleta itens por proximidade.
4. `RenderSystem` desenha as entidades com `MeshRenderer` (sprites giram em Y para a câmera); `Hud` desenha barra, mira e mensagens.
5. Se `GameEvents::exitRequested`, a `App` carrega o `next` do mapa (vida e munição passam de fase, a chave zera) ou mostra a tela de fim.

## Decisões de arquitetura

- **Manter o `Entity` simples** enquanto o número de tipos for pequeno. `Door`, `Pickup` e `PlayerStats` já entraram assim; ao adicionar `Health`, `Weapon`, `Enemy`, reavaliar migrar para um ECS por tipo **[ABERTO]** — decidir na fase de combate.
- **Mundo = grade + entidades**: a grade define paredes; objetos (inimigos, itens, portas) vêm de uma lista de entidades no arquivo de mapa.
- **Dados antes de código**: texturas, tipos de inimigo e itens vêm de catálogos em `data/`.
- **Render de paredes**: hoje um cubo por célula. Se virar gargalo, fundir faces visíveis numa malha só por mapa **[ABERTO]**.
- **Sprites**: itens (e, na Fase 3, inimigos) são billboards: quad vertical girado só em Y para a câmera, PNG com transparência descartada no fragment shader (alfa < 0,5). Sem ordenação por profundidade, já que não há semitransparência.
- **Portas**: folha fina (0,1) no meio da célula que desliza para dentro da parede vizinha; passável a partir de 90% aberta e não fecha com o jogador na célula.
- **Opções e janela**: `Settings` é carregado antes da `Window`, que já nasce com a resolução/modo salvos. O `OptionsMenu` (Ctrl) muda a sensibilidade na hora e a resolução/modo só ao APLICAR (`Window::applyVideo`); grava `settings.txt` ao aplicar e ao fechar. "Janela sem borda" usa `SDL_WINDOW_FULLSCREEN_DESKTOP` (resolução do monitor); "tela cheia" usa `SDL_WINDOW_FULLSCREEN` com o modo de vídeo mais próximo. Com o menu aberto o jogador fica parado.
- **Viewport**: `RenderSystem` refaz o `glViewport` a cada quadro com o tamanho em pixels (`SDL_GL_GetDrawableSize`), então redimensionar ou trocar de modo não distorce a imagem. HUD e menus usam o mesmo tamanho em pixels; o menu converte o mouse de coordenadas de janela para pixels.
- **UV do cubo**: as quatro faces laterais mostram a textura de pé e sem espelhar quando vistas de fora (necessário para texturas com texto, como o interruptor de saída).

## Dependências

C++14, SDL2 (sistema, via `make deps`), glad e stb_image (em `include/`/`src/`), GLM (em `third_party/`).
