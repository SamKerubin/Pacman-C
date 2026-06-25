#include "pathfinding.h"
#include "board.h"
#include "coordinates.h"
#include "ghost.h"
#include <limits.h>

coordinates find_shortest_path(uint8_t id, ghost_state state, uint8_t **board,
                               coordinates start,
                               coordinates end) {
    coordinates shortest = {-1, -1};
    int shortest_distance = INT_MAX;
    int shortest_priority = 0;

    for (int i = 0; i < 4; i++) {
        coordinates adj_coord = (coordinates){start.X + DIR_X[i], start.Y + DIR_Y[i]};

        if (adj_coord.X < 0) {
            adj_coord.X = BOARD_WIDTH - 1;
        } else if (adj_coord.X >= BOARD_WIDTH) {
            adj_coord.X = 0;
        }

        if (!is_valid_ghost_movement(adj_coord, board, id, state)) {
            continue;
        }

        int manhattan_distance = manhattan_coordinate_distance_to(adj_coord, end);
        if (manhattan_distance < shortest_distance || (manhattan_distance == shortest_distance && i < shortest_priority)) {
            shortest_distance = manhattan_distance;
            shortest = adj_coord;
            shortest_priority = i;
        }
    }

    return shortest;
}
