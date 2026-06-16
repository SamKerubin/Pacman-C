#include "pacman.h"
#include <stdlib.h>

pacman *init_pacman(uint8_t id, coordinates inital_position) {
    pacman *p = (pacman *)calloc(1, sizeof(pacman));
    p->id = id;
    p->position = inital_position;

    return p;
}

void free_pacman(pacman *pacman) {
    if (!pacman) {
        return;
    }

    free(pacman);
}
