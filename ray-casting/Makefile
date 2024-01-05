SRC_DIR = src
BUILD_DIR = build/debug
CC = gcc
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_NAME = play
INCLUDE_PATHS = -Iinclude/SDL2
LIBRARY_PATHS = -Llib/SDL2
COMPILER_FLAGS = -std=c17 -Wall -Wextra -Werror -O0 -g
LINKER_FLAGS = -lsdl2

all:
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)
