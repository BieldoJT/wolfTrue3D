# wolfTrue3D

FPS 3D inspirado em Wolfenstein 3D, cujo diferencial é **criar mundos**: mapas em grade, paredes e assets definidos por dados, editados num **editor separado**.

Documentação detalhada em `docs/`. Leia o que for relevante à tarefa:

| Doc | Quando ler |
|---|---|
| [docs/01-visao-do-jogo.md](docs/01-visao-do-jogo.md) | Entender o que é o jogo, escopo e o que está fora dele |
| [docs/02-arquitetura.md](docs/02-arquitetura.md) | Mexer em código de engine/ECS/render |
| [docs/03-formato-de-mapas.md](docs/03-formato-de-mapas.md) | Mexer em mapas, parser ou catálogos |
| [docs/04-editor.md](docs/04-editor.md) | Trabalhar no editor de mapas |
| [docs/05-assets.md](docs/05-assets.md) | Texturas, sprites, sons, catálogos |
| [docs/06-roadmap.md](docs/06-roadmap.md) | Saber o que fazer a seguir e em que ordem |
| [docs/07-convencoes.md](docs/07-convencoes.md) | Estilo de código, commits, branches |

## Comandos

```
make deps   # instala SDL2 no sistema (GLM já vem em third_party/)
make        # compila o jogo -> ./wolf
make re     # recompila do zero
```

O `Makefile` lista os `.cpp` **explicitamente** em `SRC_FILES`: ao criar um arquivo novo, adicione-o lá.

## Regras do projeto

- C++14, SDL2, OpenGL via glad, GLM. Sem dependências novas sem conversar antes.
- Mundos são **dados**, não código: conteúdo novo (mapa, textura, inimigo) deve entrar por arquivo/catálogo, sem recompilar.
- O formato de mapa é compartilhado entre jogo e editor: mudou o formato, atualize `docs/03-formato-de-mapas.md` e o parser no mesmo commit.
- Siga o roadmap: não adiantar fases sem pedir. Decisões em aberto estão marcadas como **[ABERTO]** nos docs.
- Documentação e commits em português; identificadores no código seguem o que já existe.
