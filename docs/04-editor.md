# 04 — Editor de mapas

Ferramenta **separada** do jogo (`wolf-editor`) para criar e editar mapas visualmente.

## Objetivos

- Desenhar a grade com o mouse, sem digitar texto.
- Escolher textura de cada parede a partir do catálogo.
- Posicionar spawn, inimigos, itens, portas e saída.
- Salvar e abrir `maps/*.txt` no formato de [03-formato-de-mapas.md](03-formato-de-mapas.md).
- Avisar de erros de validação (sem spawn, mapa aberto, etc.).

## Tecnologia **[ABERTO]**

Recomendação: **mesmo stack do jogo** (C++, SDL2, OpenGL), como segundo executável, reaproveitando a biblioteca de mapa e `Texture`. Alternativas: ferramenta web (HTML/JS) ou Python. Decidir antes da Fase 5 do [roadmap](06-roadmap.md).

## Interface (rascunho)

- **Área central**: grade 2D top-down com zoom e pan.
- **Paleta lateral**: texturas de parede, objetos (inimigo, item, porta, spawn, saída).
- **Ferramentas**: pincel, apagar, retângulo, balde, conta-gotas.
- **Barra**: novo / abrir / salvar, desfazer / refazer, botão **Testar** (abre o mapa no jogo).

## Fluxo

1. Novo mapa: define largura × altura, vem cercado de paredes.
2. Pinta paredes e objetos.
3. Valida e salva.
4. "Testar" executa `./wolf maps/arquivo.txt`.

## Requisitos para o jogo

O jogo precisa aceitar o mapa por argumento de linha de comando (`./wolf <mapa>`), hoje fixo em `maps/level1.txt`.

## Fora do escopo do editor v1

Visualização 3D, edição de modelos, scripts/gatilhos, multi-camadas.
