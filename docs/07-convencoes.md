# 07 — Convenções

## Código (seguir o que já existe)

- C++14, compilado com `c++` via `Makefile`.
- Indentação com **tabs**; chaves em linha própria (estilo Allman).
- Header guards no formato `GAME_MAP_GRID_HPP`, com `# define` e `# include` (espaço após `#` dentro do guard).
- Headers em `include/<camada>/`, implementação em `src/<camada>/`, espelhando a estrutura.
- Novo `.cpp`: adicionar em `SRC_FILES` no `Makefile`.
- Componentes são structs simples sem lógica; a lógica fica em `systems/`.
- Sem dependências novas sem alinhar.

## Git

- Trabalhar em branch e abrir PR para `main`; não commitar direto na `main`.
- Commits no padrão convencional, em português: `feat:`, `fix:`, `refactor:`, `docs:`, `chore:`.
- Um assunto por PR; referência ao número no título, como nos PRs existentes (`#6`).
- Mudou formato de mapa ou catálogo? Atualize o doc correspondente no mesmo PR.

## Documentação

- Decisões pendentes ficam marcadas como **[ABERTO]**; ao decidir, remova a marca e registre a escolha.
- Ao concluir um item do roadmap, marque `[x]` em [06-roadmap.md](06-roadmap.md).

## Testar

Compilar com `make` e rodar `./wolf`. Para mudanças visuais, conferir no jogo real, não só na compilação.

### Windows

No WSL2 o mouse não é capturado de verdade (o WSLg só repassa a posição absoluta do cursor do Windows), então o mouse look só funciona direito no Windows nativo. Para testar:

1. `make deps-win` (uma vez) e `make win`.
2. Copie `wolf.exe`, `SDL2.dll`, `data/`, `maps/`, `shaders/` e `images/` para uma pasta do Windows (ex.: `/mnt/c/...`) e rode `wolf.exe` a partir dela, pois os caminhos são relativos ao diretório atual.
