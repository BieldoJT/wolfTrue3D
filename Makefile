NAME = wolf

COMPILER = c++
INCLUDES = -Iinclude -I.
LIBS = -lSDL2 -ldl

SRC_DIR = src
SRC_FILES = main.cpp sdl.cpp glad.c
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

all: $(NAME)

$(NAME):
	$(COMPILER) $(INCLUDES) $(SRC) $(LIBS) -o $(NAME)
	@clear
	@echo "✅ ${NAME} is compiled."

clean:
	rm -rf $(NAME)
	@clear
	@echo "🗑️ executable removed."

re: clean all
