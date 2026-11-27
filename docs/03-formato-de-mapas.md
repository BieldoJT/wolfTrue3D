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
- Linhas que começam com letra minúscula são diretivas; diretiva desconhecida gera **aviso** e é ignorada (a grade nunca tem minúsculas).

### Validação do v0 (implementada)

Mensagens no formato `arquivo:linha: erro|aviso: texto`. **Erro** impede o carregamento; **aviso** só informa.

| Tipo | Regra |
|---|---|
| erro | Arquivo não abre |
| erro | Mapa sem grade |
| erro | Caractere inválido na grade |
| erro | `wall`/`floor`/`ceiling` sem id inteiro maior que zero, ou símbolo fora de `1`-`9` |
| erro | Nenhum spawn `P` |
| aviso | Texto extra depois do id (ignorado) |
| aviso | Diretiva desconhecida (ignorada) |
| aviso | Diretiva repetida (vale a última) |
| aviso | Mais de um `P` (vale o primeiro) |
| aviso | Grade não retangular |
| aviso | Mapa aberto: do spawn dá para andar até fora da grade |
| aviso | Id de textura que não existe em `data/textures.txt` |
| aviso | Símbolo `1`-`9` sem `wall N <id>` (usa a textura de `#`), ou `wall N` sem uso na grade |
| aviso | `#` na grade sem linha `wall <id>` (usa a textura 1) |

Para conferir mapas sem abrir o jogo: `make mapcheck` e `./mapcheck --catalog data/textures.txt maps/*.txt`. Casos de teste em `tests/maps/` (`make test`).

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

Biblioteca de formato em `include/map/` e `src/map/`, só com a biblioteca padrão (sem SDL/OpenGL), usada pelo jogo, pelo `mapcheck` e, no futuro, pelo editor:

- `map_parser` — `loadMapFile`/`parseMap`: sintaxe (diretivas e grade) → `MapData`.
- `map_validator` — `validateMap`: regras de conteúdo (spawn, mapa fechado, texturas).
- `catalog` — `readTextureCatalog`: lê `data/textures.txt` sem carregar imagens.
- `map_messages` — `MapMessage` (erro/aviso + linha) e impressão.

O serializador (salvar mapa) entra junto com o editor.
