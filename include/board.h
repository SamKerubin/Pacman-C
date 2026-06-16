#ifndef BOARD_H
#define BOARD_H

#define BOARD_WIDTH 28
#define BOARD_HEIGHT 36

#define TOTAL_DOTS 244

#define STARTER_LIFES 3

#define SCORE_PER_DOT 10
#define GHOST_EATEN_MULTIPLIER 2
#define BASE_GHOST_EATEN_SCORE 200

#define BLINKY_SCATTER_TARGET (coordinates){0, BOARD_WIDTH - 4}
#define PINKY_SCATTER_TARGET (coordinates){0, BOARD_WIDTH - (BOARD_WIDTH - 4)}
#define INKY_SCATTER_TARGET (coordinates){BOARD_HEIGHT - 2, BOARD_WIDTH - 4}
#define CLYDE_SCATTER_TARGET (coordinates){BOARD_WIDTH - 2, 0}

#define RESET "\033[0m"
#define YELLOW "\033[38;5;226m"
#define LIGHT_YELLOW "\033[38;5;229m"
#define RED "\033[38;5;196m"
#define PINK "\033[38;5;219m"
#define CYAN "\033[38;5;51m"
#define ORANGE "\033[38;5;214m"
#define BLUE "\033[38;5;20m"

#define PACMAN_ICON_UP "v"
#define PACMAN_ICON_DOWN "^"
#define PACMAN_ICON_LEFT ">"
#define PACMAN_ICON_RIGHT "<"

#define DOT_ICON "•"
#define POWER_DOT_ICON "O"

#define GHOST_ICON "^"

#define WALL_ICON "#"

#define LIFE_ICON PACMAN_ICON_LEFT

#define SCORE_FILE_PATH "/home/%s/.config/pacman-score.txt"

#include "coordinates.h"
#include "pacman.h"
#include "ghost.h"
#include <stdint.h>

typedef enum entity_id {
    WALL_ID = 0,
    EMPTY_ID,
    DOT_ID,
    POWER_DOT_ID,
    PACMAN_ID,
    BLINKY_ID,
    PINKY_ID,
    INKY_ID,
    CLYDE_ID
} entity_id;

typedef struct board {
    uint8_t **board;
    pacman *pacman;
    ghost *blinky;
    ghost *pinky;
    ghost *inky;
    ghost *clyde;
    ghost *preferred_ghost;
    // coordinates power_dots_positions[TOTAL_POWER_DOTS];
    // uint8_t ghosts_eaten;
    // uint8_t power_dots_count;
    uint8_t dots_eaten;
    uint32_t score;
    uint16_t lifes;
    uint16_t level;
} board;

board *init_board();
void end_game(board *board);

int is_valid(coordinates coord,
                    uint8_t **board,
                    uint8_t id);

void update_board(board *board);
void print_board(board *board);

void move_pacman(board *board);
void move_ghosts(board *board);

void eat_ghost(board *board);
void eat_dot(board *board);
void eat_power_dot(board *board);

coordinates get_blinky_target_position(board *board);
coordinates get_pinky_target_position(board *board);
coordinates get_inky_target_position(board *board);
coordinates get_clyde_target_position(board *board);

void life_lost(board *board);
void go_next_level(board *board);

#endif // !BOARD_H

