#include "ghost.h"
#include "coordinates.h"
#include "pathfinding.h"
#include <stdlib.h>

static void ghost_init_pattern(ghost *ghost) {
    if (ghost->current_direction == UP) {
        ghost->current_direction = DOWN;
    } else {
        ghost->current_direction = UP;
    }
}

static void ghost_execute_pattern(ghost *ghost, uint8_t **board) {
    coordinates shortest = find_shortest_path(ghost->id, board,
                                              ghost->position,
                                              ghost->target_coordinate);

    ghost->next_position = shortest;
}

ghost *init_ghost(uint8_t id, coordinates init_position) {
    ghost *g = (ghost *)calloc(1, sizeof(ghost));
    g->id = id;
    g->position = init_position;
    g->next_position = init_position;

    return g;
}

void free_ghost(ghost *ghost) {
    if (!ghost) {
        return;
    }

    free(ghost);
}

void move_ghost(ghost *ghost, uint8_t **board, coordinates target) {
    ghost->position = ghost->next_position;
    board[ghost->position.Y][ghost->position.X] = ghost->id;
    ghost->target_coordinate = target;

    if (ghost->state == INIT) {
        ghost_init_pattern(ghost);
        return;
    }

    ghost_execute_pattern(ghost, board);
}

