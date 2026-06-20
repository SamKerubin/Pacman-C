#ifndef PACMAN_H
#define PACMAN_H

#include "coordinates.h"
#include <stdint.h>

typedef struct pacman {
    coordinates position;
    coordinates last_position;
    direction current_direction;
    uint8_t id;
    int64_t next_movement_time;
} pacman;

pacman *init_pacman(uint8_t id, coordinates inital_position);
void free_pacman(pacman *pacman);

#endif // !PACMAN_H
