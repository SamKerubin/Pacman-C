CC = gcc

INC_FLAGS = -Iinclude/
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -O0 $(INC_FLAGS)
COBJFLAGS = $(CFLAGS) -c

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

SRC = $(shell find $(SRC_DIR) -name "*.c" -type f)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

BIN = $(BIN_DIR)/pacman-c

default: makedir all

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(COBJFLAGS) -MMD -MP -MF $(@:.o=.d) -o $@ $<

-include $(OBJ_DIR)/*.d

.PHONY: makedir
makedir:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

.PHONY: all
all: $(BIN) run

.PHONY: clean
clean:
	@rm -r $(BIN_DIR) $(OBJ_DIR)

.PHONY: rebuild
rebuild: clean default

run:
	@./$(BIN)
