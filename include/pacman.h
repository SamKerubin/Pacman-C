#ifndef PACMAN_H
#define PACMAN_H

#include "coordinates.h"
#include <stdint.h>

typedef struct pacman {
    coordinates position;
    direction current_direction;
    uint8_t id;
} pacman;

pacman *init_pacman(coordinates inital_position);
void free_pacman(pacman *pacman);

#endif // !PACMAN_H
