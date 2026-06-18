#include "ghost.h"
#include "board.h"
#include "coordinates.h"
#include "pathfinding.h"
#include <stdlib.h>

ghost *init_ghost(uint8_t id, coordinates init_position) {
    ghost *g = (ghost *)calloc(1, sizeof(ghost));
    g->id = id;
    g->position = init_position;
    g->next_position = init_position;
    g->current_direction = UP;

    return g;
}

void free_ghost(ghost *ghost) {
    if (!ghost) {
        return;
    }

    free(ghost);
}

static inline int can_enter_home(ghost_state state) {
    return state == EATEN || state == OUT_OF_HOUSE;
}

static inline int can_turn(ghost_state state) {
    return state == INIT;
}

int is_valid_ghost_movement(coordinates coord, uint8_t ** board, uint8_t ghost_id, ghost_state state) {
    int valid = !is_wall(coord, board);

    if (!can_turn(state)) {
        valid = valid && board[coord.Y][coord.X] != ghost_id;
    }

    if (!can_enter_home(state)) {
        valid = valid && board[coord.Y][coord.X] != HOME_DOOR_ID;
    }

    return valid;
}

void move_ghost(ghost *ghost, uint8_t **board, coordinates target) {
    // TODO: Set ghost direction based on current and last position
    ghost->last_position = ghost->position;
    ghost->position = ghost->next_position;

    if (!is_inside_bounds(ghost->position)) {
        ghost->position.X = (ghost->position.X % BOARD_HEIGHT + BOARD_HEIGHT) % BOARD_HEIGHT;
        ghost->position.Y = (ghost->position.Y % BOARD_WIDTH + BOARD_WIDTH) % BOARD_WIDTH;
    }

    ghost->target_coordinate = target;

    ghost->next_position = find_shortest_path(ghost->id, ghost->state,
                                              board, ghost->position,
                                              ghost->target_coordinate);
}
