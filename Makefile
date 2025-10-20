# ----------------------------------------Name
NAME = ft_vox

# ----------------------------------------Files
FILES_SRCS = \
			main.cpp \
			VoxEngine.cpp \
			Camera.cpp \
			Input.cpp \
			Block.cpp \
			Chunk.cpp \
			Perlin.cpp \
			ChunkManager.cpp \
			ChunkMesh.cpp \
			Spline.cpp \
			Atlas.cpp \
			Plane.cpp \
			AABB.cpp \
			Frustum.cpp \
			Renderer.cpp \
			Player.cpp \
			Coords.cpp \

FILES_OBJS = $(FILES_SRCS:.cpp=.o)

# ----------------------------------------Directories
DIR_SRCS = ./src/
DIR_OBJS = ./obj/
DIR_LIB = ./lib/
DIR_GLU = $(DIR_LIB)glu/
DIR_JSP = $(DIR_LIB)json-parser/

vpath %.cpp \
	$(DIR_SRCS) \

# ----------------------------------------Sources
SRCS = $(FILES_SRCS:%=$(DIR_SRCS)%)

# ----------------------------------------Objects
OBJS = $(FILES_OBJS:%=$(DIR_OBJS)%)

# ----------------------------------------Libs
GLU = $(DIR_GLU)libgl-utils.a
GLU_SM = $(DIR_GLU).git

JSP = $(DIR_JSP)libjson-parser.a
JSP_SM = $(DIR_JSP).git

# ----------------------------------------Flags
CC = c++
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -O3
CFLAGS += -std=c++20
# CFLAGS += -fsanitize=address -g
# CFLAGS += -fsanitize=thread -g
LFLAGS = -lglfw

TSAN_OPTIONS="suppressions=tsan-suppression"
# TSAN_OPTIONS="suppressions=tsan-suppression history_size=7"

INC = \
	-Iinc \
	-I$(DIR_LIB) \

all:
	@$(MAKE) $(NAME) -j4
.PHONY: all

$(NAME): $(GLU) $(JSP) $(DIR_OBJS) $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(GLU) $(JSP) $(CFLAGS) $(LFLAGS)

$(DIR_OBJS)%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(DIR_OBJS):
	mkdir -p $@

$(GLU): $(GLU_SM)
	$(MAKE) -C $(DIR_GLU)

$(GLU_SM):
	@$(MAKE) submodule

$(JSP): $(JSP_SM)
	$(MAKE) -C $(DIR_JSP)

$(JSP_SM):
	@$(MAKE) submodule

submodule:
	@echo "fetching submodules..."
	git submodule init
	git submodule update
.PHONY: submodule

run-tsan:
	export TSAN_OPTIONS=$(TSAN_OPTIONS); \
	./$(NAME)
.PHONY: run-tsan

lines:
	wc -l src/*.cpp inc/*.hpp
# 	wc -l src/*.cpp inc/*.hpp lib/glu/src/*.cpp lib/json-parser/src/*.cpp lib/glu/inc/utils/*.hpp lib/glu/inc/bmp/bmp.h lib/glu/lib/bmp/bmp.cpp lib/glu/lib/emlm/src/*.cpp lib/glu/lib/emlm/mlm/*.hpp
.PHONY: lines
# ----------------------------------------Cleaning
clean:
	rm -f $(OBJS)
.PHONY: clean

fclean: clean
	rm -f $(NAME)
.PHONY: fclean

re: fclean all
.PHONY: re

libs:
	$(MAKE) re -C $(DIR_GLU)
	$(MAKE) re -C $(DIR_JSP)
.PHONY: libs
