#include "ghost.h"
#include "board.h"
#include "coordinates.h"
#include "pathfinding.h"
#include <stdio.h>
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

void move_ghost(ghost *ghost, uint8_t **board, coordinates target) {
    ghost->last_position = ghost->position;
    ghost->position = ghost->next_position;

    if (!is_inside_bounds(ghost->position)) {
        ghost->position.X = (ghost->position.X % BOARD_HEIGHT + BOARD_HEIGHT) % BOARD_HEIGHT;
        ghost->position.Y = (ghost->position.Y % BOARD_WIDTH + BOARD_WIDTH) % BOARD_WIDTH;
    }

    ghost->target_coordinate = target;

    ghost->next_position = find_shortest_path(ghost->id, (ghost->state != INIT && ghost->state != EATEN && ghost->state != OUT_OF_HOUSE),
                                              board, ghost->position,
                                              ghost->target_coordinate);
    printf("next: %d -> %d, %d\n", ghost->id, ghost->next_position.Y, ghost->next_position.X);
    printf("current: %d -> %d, %d\n", ghost->id, ghost->position.Y, ghost->position.X);
    printf("target: %d -> %d, %d\n", ghost->id, ghost->target_coordinate.Y, ghost->target_coordinate.X);
}

    }

    ghost_execute_pattern(ghost, board);
}

