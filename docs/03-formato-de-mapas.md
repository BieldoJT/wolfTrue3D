# 03 — Formato de mapas

Mapas são arquivos de texto em `maps/`. Devem ser legíveis, editáveis à mão e geráveis pelo editor.

## Formato atual (v0)

```
// comentário
wall 1
floor 1
ceiling 1
############
#..........#
#....##....#
#..P.......#
############
```

- Linhas começando com `//` são comentários.
- `wall <id>`: textura das paredes, id do catálogo `data/textures.txt`.
- `floor <id>` / `ceiling <id>`: textura do chão e do teto (ids do catálogo; opcionais, padrão `1`). A textura se repete uma vez por célula.
- `wall <símbolo> <id>`: textura das paredes de símbolo `1`-`9` (várias texturas no mesmo mapa). Símbolo sem linha `wall` usa a textura de `#` e gera aviso.
- Grade: `#` parede (textura de `wall <id>`), `1`-`9` parede com textura própria, `.` ou espaço vazio, `P` spawn do jogador.

## Formato proposto (v1) **[ABERTO — validar antes de implementar]**

Mantém a grade legível e acrescenta cabeçalho e lista de objetos.

```
// nome do mapa
name Primeiro Andar
version 1

// textura por símbolo de parede (id do catálogo)
wall 1 1        // símbolo '1' -> textura 1
wall 2 2        // símbolo '2' -> textura 2

grid
1111111111
1P.......1
1...22...1
1.......D1
1111111111
end

// objetos fora da grade: tipo, coluna, linha, extras
enemy guard 5 2
item ammo 3 1
item key_blue 7 3
door 8 3 key=key_blue
exit 8 1
```

### Regras

- Símbolos de parede `1`-`9` apontam para texturas diferentes (várias paredes no mesmo mapa).
- `.` vazio, `P` spawn (exatamente um por mapa).
- Coordenadas de objetos são `coluna linha` (origem no canto superior esquerdo).
- Linhas desconhecidas geram **aviso**, não erro fatal, para manter compatibilidade entre versões.
- `version` permite migrar formatos antigos; mapas v0 (sem `version`) continuam carregando.

### Validação (parser e editor)

- Exatamente um spawn.
- Grade retangular e fechada por paredes (aviso se houver abertura para fora).
- Ids de textura e tipos de objeto existem nos catálogos.
- Objetos dentro dos limites e fora de células de parede.

## Onde fica o código

Parser/serializador numa biblioteca única usada por jogo e editor (hoje o carregamento está em `src/game/map_grid.cpp` e `scene.cpp`; extrair quando o editor começar).
