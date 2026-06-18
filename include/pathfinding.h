#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "coordinates.h"
#include "ghost.h"
#include <stdint.h>

coordinates find_shortest_path(uint8_t id, ghost_state state, uint8_t **board,
                               coordinates start,
                               coordinates end);

#endif // !PATHFINDING_H
