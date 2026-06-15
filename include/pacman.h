#ifndef PACMAN_H
#define PACMAN_H

#include "coordinates.h"

typedef struct pacman {
    coordinates position;
    direction current_direction;
} pacman;

pacman *init_pacman(coordinates inital_position);
void free_pacman(pacman *pacman);

#endif // !PACMAN_H
