# 06 — Roadmap

Marque `[x]` ao concluir. Cada fase deve terminar jogável/testável. Não adiantar fases sem alinhar.

## Fase 0 — Base (feito)

- [x] Janela SDL2 + contexto OpenGL
- [x] Shaders, malha de cubo, texturas
- [x] Câmera FPS com mouse look livre
- [x] Mapa em grade carregado de arquivo + catálogo de texturas
- [x] Colisão do jogador com paredes
- [x] `make deps` e GLM vendido

## Fase 1 — Mundo sólido

- [ ] Chão e teto renderizados
- [ ] Várias texturas de parede no mesmo mapa (símbolos `1`-`9`)
- [ ] Jogo aceita mapa por argumento (`./wolf <mapa>`)
- [ ] Parser com validação e avisos (ver [03](03-formato-de-mapas.md))
- [ ] Extrair parser para biblioteca compartilhada

## Fase 2 — Interação

- [ ] Portas (abrem/fecham)
- [ ] Itens coletáveis (munição, vida, chaves)
- [ ] Saída de fase e troca de mapa
- [ ] HUD mínimo (vida, munição)

## Fase 3 — Combate

- [ ] Arma hitscan com tiro, recarga e cadência
- [ ] Sprites billboard para inimigos
- [ ] Vida e dano (jogador e inimigos)
- [ ] IA simples: patrulhar, ver o jogador, perseguir, atirar
- [ ] Catálogos `enemies`, `items`, `weapons`

## Fase 4 — Formato v1 de mapa

- [ ] Lista de objetos no mapa (inimigos, itens, portas, saída)
- [ ] Versionamento e migração de mapas v0
- [ ] Fechar o formato e atualizar [03](03-formato-de-mapas.md)

## Fase 5 — Editor

- [ ] Decidir tecnologia do editor ([04](04-editor.md))
- [ ] Grade 2D com pincel e paleta de texturas
- [ ] Posicionar objetos
- [ ] Salvar/abrir, validar, desfazer
- [ ] Botão "Testar" no jogo

## Fase 6 — Polimento e criação

- [ ] Menu de seleção de mapas
- [ ] Sons básicos
- [ ] Importar texturas próprias pelo editor
- [ ] Documentação para criadores (como fazer seu mundo)

## Depois (backlog)

Pacotes de mundo compartilháveis, modelos 3D e sons importáveis, edição in-game, multiplayer.

## Ordem sugerida

A Fase 1 destrava tudo (formato + parser compartilhado). Combate (3) e editor (5) dependem do formato v1 (4): se quiser o editor cedo, faça 1 → 4 → 5 e deixe o combate para depois.
