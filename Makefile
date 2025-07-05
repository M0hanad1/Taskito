# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Iinclude

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Files
TARGET := $(BIN_DIR)/app
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Add debugging
debug: CFLAGS += -g
debug: $(TARGET)

# Clean build files
clean:
	rm -rf obj/* bin/*

# Optional: rebuild everything
rebuild: clean all

# Prevent make from deleting intermediate files
.PRECIOUS: $(OBJ)
.PHONY: all run clean rebuild
