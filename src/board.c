#include "board.h"
#include "coordinates.h"
#include "ghost.h"
#include "pacman.h"
#include <stdio.h>
#include <stdlib.h>

static void set_walls(board *board) {

}

board *init_board() {
    board *b = (board *)calloc(1, sizeof(board));

    b->board = (uint8_t **)malloc(BOARD_HEIGHT * sizeof(uint8_t *));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        b->board[i] = (uint8_t *)malloc(BOARD_WIDTH * sizeof(uint8_t));
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            b->board[i][j] = EMPTY_ID;
        }
    }

    set_walls(b);

    b->blinky = init_ghost(BLINKY_ID, (coordinates){});
    b->blinky->state = SCATTER;

    b->board[b->blinky->position.Y][b->blinky->position.X] = b->blinky->id;

    b->pinky = init_ghost(PINKY_ID, (coordinates){});
    b->inky = init_ghost(INKY_ID, (coordinates){});
    b->clyde = init_ghost(CLYDE_ID, (coordinates){});

    b->board[b->pinky->position.Y][b->pinky->position.X] = b->pinky->id;
    b->board[b->inky->position.Y][b->inky->position.X] = b->inky->id;
    b->board[b->clyde->position.Y][b->clyde->position.X] = b->clyde->id;

    b->pacman = init_pacman((coordinates){});

    return b;
}

void end_game(board *board) {

}

int is_valid(coordinates coord,
                    uint8_t **board,
                    uint8_t id) {
    return is_inside_bounds(coord)
            && !is_wall(coord, board)
            && board[coord.Y][coord.X] != id;
}

}

void print_board(board *board) {

}

void move_pacman(board *board) {

}

void move_ghosts(board *board) {

}

void eat_ghost(board *board) {

}

void eat_dot(board *board) {

}

void eat_power_dot(board *board) {

}

static int generate_pseudo_random_number(int range) {
    return rand() % range;
}

static coordinates get_random_valid_adjacent(coordinates start, uint8_t **board, uint8_t id) {
    coordinates adj[4] = {0};
    for (int i = 0; i < 4; i++) {
        adj[i] = (coordinates){start.X + DIR_X[i], start.Y + DIR_Y[i]};
    }

    int r = -1;
    do {
        r = rand() % 4;
    } while(!is_valid(adj[r], board, id));

    return adj[r];
}

coordinates get_blinky_target_position(board *board) {
    switch (board->blinky->state) {
        case SCATTER: return BLINKY_SCATTER_TARGET;
        case CHASE: return board->pacman->position;
        case FRIGHTENED: return get_random_valid_adjacent(board->blinky->position,
                                                          board->board,
                                                          board->blinky->id); 
        case INIT:
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates pinky_chase_target(board *board) {
    const coordinates OFFSET = {4, 4};
 
    coordinates pacman_pos = board->pacman->position;
    direction pacman_dir = board->pacman->current_direction;

    coordinates direction = (coordinates){DIR_X[pacman_dir], DIR_Y[pacman_dir]};
    coordinates prod = coordinates_prod(OFFSET, direction);

    return coordinates_sum(pacman_pos, prod);
}

coordinates get_pinky_target_position(board *board) {
    switch (board->pinky->state) {
        case INIT: return board->pinky->position;
        case SCATTER: return PINKY_SCATTER_TARGET;
        case CHASE: return pinky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->pinky->position,
                                                          board->board,
                                                          board->pinky->id); 
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates inky_chase_target(board *board) {
    const coordinates OFFSET = {2, 2};

    coordinates pacman_pos = board->pacman->position;
    direction pacman_dir = board->pacman->current_direction;

    coordinates direction = {DIR_X[pacman_dir], DIR_Y[pacman_dir]};
    coordinates prod = coordinates_prod(OFFSET, direction);

    coordinates pacman_offset = coordinates_sum(pacman_pos, prod);

    coordinates blinky_pos = board->blinky->position;

    coordinates diff = coordinates_diff(blinky_pos, pacman_offset);
    coordinates mult = coordinates_mult(diff, 2);

    return coordinates_sum(mult, blinky_pos);
}

coordinates get_inky_target_position(board *board) {
    switch (board->inky->state) {
        case INIT: return board->inky->position;
        case SCATTER: return INKY_SCATTER_TARGET;
        case CHASE: return inky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->inky->position,
                                                          board->board,
                                                          board->inky->id); 
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates clyde_chase_target(board *board) {
    if (coordinate_distance_to(board->pacman->position, board->clyde->position) > 8.0f) {
        return board->pacman->position;
    }

    return CLYDE_SCATTER_TARGET;

}

coordinates get_clyde_target_position(board *board) {
    switch (board->clyde->state) {
        case INIT: return board->clyde->position;
        case SCATTER: return CLYDE_SCATTER_TARGET;
        case CHASE: return clyde_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->clyde->position,
                                                          board->board,
                                                          board->clyde->id); 
        default:
            return (coordinates){-1, -1};

    }
}

void life_lost(board *board) {
    board->lifes--;
}

void go_next_level(board *board) {
    board->level++;
}

