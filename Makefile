NAME = wolf

COMPILER = c++
CXXFLAGS = -std=c++14
# GLM é header vendido. SDL2 e OpenGL ficam no sistema (make deps).
INCLUDES = -Iinclude -I. -Ithird_party/glm
SRC_DIR = src
SRC_FILES = main.cpp glad.c stb_image.cpp \
	platform/window.cpp \
	platform/input.cpp \
	render/shader.cpp \
	render/mesh.cpp \
	render/texture.cpp \
	game/scene.cpp \
	game/map_grid.cpp \
	game/systems/camera_system.cpp \
	game/systems/map_build_system.cpp \
	game/systems/render_system.cpp \
	ui/sensitivity_menu.cpp \
	app/app.cpp
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

.PHONY: all clean re deps deps-win win

all: $(NAME)

deps:
	bash scripts/setup-deps.sh

# Build para Windows (cross-compile com MinGW a partir do Linux/WSL).
# Gera wolf.exe + SDL2.dll na raiz; rode no Windows com data/, maps/ e shaders/ ao lado.
WIN_CXX = x86_64-w64-mingw32-g++
WIN_SDL = third_party/SDL2-win/x86_64-w64-mingw32
WIN_EXE = $(NAME).exe

deps-win:
	bash scripts/setup-deps-win.sh

win: $(SRC)
	@if ! command -v $(WIN_CXX) >/dev/null 2>&1 || [ ! -d $(WIN_SDL) ]; then \
		echo 'Faltam MinGW ou SDL2 para Windows. Rode: make deps-win && make win'; \
		exit 1; \
	fi
	$(WIN_CXX) $(CXXFLAGS) $(INCLUDES) -I$(WIN_SDL)/include/SDL2 $(SRC) -L$(WIN_SDL)/lib \
		-lmingw32 -lSDL2main -lSDL2 -lopengl32 -static-libgcc -static-libstdc++ -mconsole -o $(WIN_EXE)
	cp $(WIN_SDL)/bin/SDL2.dll .
	@echo "✅ ${WIN_EXE} compilado (copie wolf.exe, SDL2.dll, data/, maps/, shaders/ e images/ para o Windows)."

$(NAME): $(SRC)
	@if ! pkg-config --exists sdl2 >/dev/null 2>&1; then \
		echo 'SDL2 não está instalado. Rode: make deps && make'; \
		exit 1; \
	fi
	@if ! pkg-config --exists gl >/dev/null 2>&1; then \
		echo 'OpenGL não está instalado. Rode: make deps && make'; \
		exit 1; \
	fi
	$(COMPILER) $(CXXFLAGS) $(INCLUDES) $$(pkg-config --cflags sdl2) $(SRC) $$(pkg-config --libs sdl2) $$(pkg-config --libs gl) -ldl -o $(NAME)
	@clear >/dev/null 2>&1 || true
	@echo "✅ ${NAME} is compiled."

clean:
	rm -rf $(NAME) $(WIN_EXE) SDL2.dll
	@clear >/dev/null 2>&1 || true
	@echo "🗑️ executable removed."

re: clean all
