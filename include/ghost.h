#ifndef GHOST_H
#define GHOST_H

#define PINKY_DOT_LIMIT 0
#define INKY_DOT_LIMIT 30
#define CLYDE_DOT_LIMIT 60

#include "coordinates.h"
#include <stdint.h>

typedef enum ghost_state {
    INIT = 0,
    OUT_OF_HOUSE,
    SCATTER,
    CHASE,
    FRIGHTENED,
    EATEN
} ghost_state;

typedef struct ghost {
    coordinates position;
    coordinates last_position;
    coordinates next_position;
    direction current_direction;
    coordinates target_coordinate;
    ghost_state state;
    uint8_t id;
    uint8_t dot_counter;
    uint8_t dot_limit;
} ghost;

ghost *init_ghost(uint8_t id, coordinates init_position);
void free_ghost(ghost *ghost);

int is_valid_ghost_movement(coordinates coord, uint8_t ** board, uint8_t ghost_id, ghost_state state);

void move_ghost(ghost *ghost, uint8_t **board, coordinates target);

#endif // !GHOST_H
