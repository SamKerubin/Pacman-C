#ifndef BOARD_H
#define BOARD_H

#define BOARD_WIDTH 28
#define BOARD_HEIGHT 36

#define TOTAL_DOTS 240
#define TOTAL_POWER_DOTS 4

#define STARTER_LIFES 3

#define SCORE_PER_DOT 10
#define GHOST_EATEN_MULTIPLIER 2
#define BASE_GHOST_EATEN_SCORE 200

#define BLINKY_SCATTER_TARGET (coordinates){BOARD_WIDTH - 3, 0}
#define PINKY_SCATTER_TARGET (coordinates){BOARD_WIDTH - (BOARD_WIDTH - 2), 0}
#define INKY_SCATTER_TARGET (coordinates){BOARD_WIDTH - 1, BOARD_HEIGHT - 2}
#define CLYDE_SCATTER_TARGET (coordinates){0, BOARD_HEIGHT - 2}

#define PACMAN_INIT_POSITION (coordinates){13, 26}

#define IN_HOME_POSITION (coordinates){13, 16}
#define OUT_HOME_POSITION (coordinates){13, 14}

#define BLINKY_INIT_POSITION (coordinates){13, 14}
#define PINKY_INIT_POSITION (coordinates){13, 17}
#define INKY_INIT_POSITION (coordinates){11, 17}
#define CLYDE_INIT_POSITION (coordinates){15, 17}

#define RESET "\033[0m"
#define BLACK_BACKGROUND "\033[48;5;16m"
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
#define POWER_DOT_ICON "@"

#define GHOST_ICON "^"

#define WALL_ICON "#"
#define HOME_DOOR_ICON "-"

#define LIFE_ICON PACMAN_ICON_LEFT

#define FRIGHTENED_TIME_MS 8000
#define PREFERRED_GHOST_AUTOMATIC_MOVE_TIME_MS 4000

#define SCORE_FILE_PATH "/home/%s/.config/pacman-score.txt"

#include "coordinates.h"
#include "pacman.h"
#include "ghost.h"
#include <stdint.h>

typedef enum entity_id {
    WALL_ID = 0,
    HOME_DOOR_ID,
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
    ghost *preferred_ghost[3];
    coordinates dots_positions[TOTAL_DOTS];
    coordinates power_dots_positions[TOTAL_POWER_DOTS];
    uint8_t remaining_dots;
    uint8_t remaining_power_dots;
    uint32_t score;
    uint16_t lifes;
    uint16_t level;
    uint8_t global_dot_counter;
    uint8_t *current_counter_reference;
    uint8_t current_ghost;
    int64_t last_dot_eaten_time;
    uint8_t has_eaten_power_dot;
    int64_t power_dot_time;
} board;

board *init_board();
void end_game(board *board);

int is_inside_bounds(coordinates coord);
int is_wall(coordinates coord, uint8_t **board);

void update_ghosts(board *board);
void update_board(board *board);
void print_board(board *board);

int move_pacman(board *board, direction d);

void eat_ghost(board *board);
void eat_dot(coordinates dot_pos, board *board);
void eat_power_dot(coordinates pow_pos, board *board);

coordinates get_blinky_target_position(board *board);
coordinates get_pinky_target_position(board *board);
coordinates get_inky_target_position(board *board);
coordinates get_clyde_target_position(board *board);

void life_lost(board *board);
void go_next_level(board *board);

#endif // !BOARD_H

