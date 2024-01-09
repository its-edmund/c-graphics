SRC_DIR = src
BUILD_DIR = build/debug
CC = gcc
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_NAME = play
INCLUDE_PATHS = -Iinclude
LIBRARY_PATHS = -Llib
CFLAGS = -std=c17 -Wall -Wextra -Werror -O0 -g
LDFLAGS = -lglfw3

all:
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LDFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
