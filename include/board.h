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

#define SCORE_FILE_PATH "/home/%s/.config/pacman-score.txt"

#include "coordinates.h"
#include "pacman.h"
#include "ghost.h"

typedef struct board {
    char **board;
    pacman *pacman;
    ghost *blinky;
    ghost *pinky;
    ghost *inky;
    ghost *clyde;
    ghost **preferred_ghost;
    coordinates power_dots_positions[TOTAL_POWER_DOTS];
    int ghosts_eaten;
    int power_dots_count;
    int score;
    int dots_eaten;
    int lifes;
    int level;
} board;

board *init_board();
void end_game(board *board);

void update_board(board *board);

void move_pacman(board *board);
void move_ghosts(board *board);

void eat_ghost(board *board);
void eat_dot(board *board);
void eat_power_dot(board *board);

void life_lost(board *board);
void go_next_level(board *board);

#endif // !BOARD_H

