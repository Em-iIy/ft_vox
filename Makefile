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

FILES_OBJS = $(FILES_SRCS:.cpp=.o)

# ----------------------------------------Directories
DIR_SRCS = ./src/
DIR_OBJS = ./obj/
DIR_LIB = ./lib/
DIR_GLU = $(DIR_LIB)glu/

vpath %.cpp \
	$(DIR_SRCS) \

# ----------------------------------------Sources
SRCS = $(FILES_SRCS:%=$(DIR_SRCS)%)

# ----------------------------------------Objects
OBJS = $(FILES_OBJS:%=$(DIR_OBJS)%)

# ----------------------------------------Libs
GLU = $(DIR_GLU)libgl-utils.a
GLU_SM = $(DIR_GLU).git

# ----------------------------------------Flags
CC = c++
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -O3
CFLAGS += -g
CFLAGS += -std=c++20
# CFLAGS += -fsanitize=address -g
LFLAGS = -lglfw

INC = \
	-Iinc \
	-I$(DIR_LIB) \

all:
	@$(MAKE) $(NAME) -j4
.PHONY: all

$(NAME): $(GLU) $(DIR_OBJS) $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(GLU) $(CFLAGS) $(LFLAGS)

$(DIR_OBJS)%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(DIR_OBJS):
	mkdir -p $@

$(GLU): $(GLU_SM)
	$(MAKE) -C $(DIR_GLU)

$(GLU_SM):
	@$(MAKE) submodule

submodule:
	@echo "fetching submodules..."
	git submodule init
	git submodule update

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
.PHONY: libs
