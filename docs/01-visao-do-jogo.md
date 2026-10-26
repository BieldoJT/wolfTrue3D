# 01 — Visão do jogo

## Em uma frase

Um FPS em 3D no estilo Wolfenstein 3D (mapas em grade, tiro, inimigos, portas e chaves) em que **qualquer pessoa pode criar e jogar os próprios mundos**.

## Pilares

1. **Sensação de Wolfenstein**: corredores, salas, tiro direto, ritmo rápido, câmera em primeira pessoa.
2. **Mundos como dados**: um mapa é um arquivo de texto legível. Criar conteúdo não exige recompilar nem programar.
3. **Criação acessível**: um editor separado, simples, para montar mapas visualmente.
4. **Simplicidade técnica**: poucos sistemas, bem separados, fáceis de estender.

## Loop de jogo (jogador)

Escolhe um mapa → spawna → explora → encontra chaves/portas → enfrenta inimigos → pega munição/vida → alcança a saída.

## Loop de criação (criador)

Abre o editor → desenha paredes na grade → escolhe texturas do catálogo → posiciona spawn, inimigos, itens, portas → salva `maps/*.txt` → testa no jogo.

## Escopo

### Dentro (v1)

- Mapas em **grade** com paredes de texturas variadas.
- Editor **separado** (ferramenta 2D top-down) que lê e grava o formato de mapa.
- Combate em primeira pessoa: armas **hitscan**, inimigos com IA simples, vida, munição.
- Portas, chaves e saída de fase.
- Catálogos de assets (`data/`) que o criador estende adicionando arquivos e linhas.

### Fora (por enquanto)

- Multiplayer.
- Edição dentro do jogo (modo build in-game).
- Importar modelos 3D ou sons próprios (v1 cobre só texturas; o resto vem depois).
- Compartilhar mundos em pacote/online.
- Paredes não alinhadas à grade, andares múltiplos, rampas.

## Estado atual (resumo)

Já existe: janela SDL2, render OpenGL de cubos, câmera com mouse look, colisão contra paredes da grade, mapa carregado por argumento (`./wolf <mapa>`, padrão `maps/level1.txt`) e catálogo de texturas. Veja o [roadmap](06-roadmap.md) para o resto.
