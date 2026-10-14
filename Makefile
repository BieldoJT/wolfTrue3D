NAME = wolf

COMPILER = c++
CXXFLAGS = -std=c++14
# GLM é header vendido. SDL2 e OpenGL ficam no sistema (make deps).
INCLUDES = -Iinclude -I. -Ithird_party/glm
LIBS = -lSDL2 -ldl

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
	app/app.cpp
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

.PHONY: all clean re deps

all: $(NAME)

deps:
	bash scripts/setup-deps.sh

$(NAME): $(SRC)
	$(COMPILER) $(CXXFLAGS) $(INCLUDES) $(SRC) $(LIBS) -o $(NAME)
	@clear >/dev/null 2>&1 || true
	@echo "✅ ${NAME} is compiled."

clean:
	rm -rf $(NAME)
	@clear >/dev/null 2>&1 || true
	@echo "🗑️ executable removed."

re: clean all
