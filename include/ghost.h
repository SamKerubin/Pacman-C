#ifndef GHOST_H
#define GHOST_H

#define PINKY_DOT_LIMIT 0
#define INKY_DOT_LIMIT 30
#define CLYDE_DOT_LIMIT 60

#include "coordinates.h"

typedef enum ghost_state {
    INIT = 0,
    SCATTER,
    CHASE,
    FRIGHTENED,
    EATEN
} ghost_state;

typedef struct ghost {
    coordinates position;
    coordinates next_position;
    void (*init_tile_finder)(struct ghost *self, void *board);
    void (*scatter_tile_finder)(struct ghost *self, void *board);
    void (*chase_tile_finder)(struct ghost *self, void *board);
    void (*frightened_tile_finder)(struct ghost *self, void *board);
    void (*eaten_tile_finder)(struct ghost *self, void *board);
    coordinates target_coordinate;
    ghost_state state;
    int dot_counter;
} ghost;

ghost *init_ghost(coordinates init_position,
                  void (*chase_tile_finder)(ghost *self, void *board),
                  void (*scatter_tile_finder)(ghost *self, void *board),
                  void (*frightened_tile_finder)(ghost *self, void *board));
void free_ghost(ghost *ghost);

void move_ghost(ghost *ghost);

#endif // !GHOST_H
