#include "pathfinding.h"
#include "board.h"
#include "coordinates.h"
#include <math.h>

static int is_valid(coordinates coord,
                    uint8_t **board,
                    uint8_t id) {
    return (coord.X < BOARD_HEIGHT && coord.X >= 0)
            && (coord.Y < BOARD_WIDTH && coord.Y >= 0)
            && board[coord.Y][coord.X] != WALL_ID
            && board[coord.Y][coord.X] != id;
}

coordinates find_shortest_path(uint8_t id, uint8_t **board,
                               coordinates start,
                               coordinates end) {
    if (compare_coordinates(start, end)) {
        return start;
    }

    coordinates shortest = {-1, -1};
    float shortest_distance = INFINITY;
    for (int i = 0; i < 4; i++) {
        coordinates adj_coord = (coordinates){start.Y + DIR_Y[i], start.X + DIR_X[i]};

        if (!is_valid(adj_coord, board, id)) {
            continue;
        }

        float eucledian_distance = coordinate_distance_to(adj_coord, end);
        if (eucledian_distance < shortest_distance) {
            shortest_distance = eucledian_distance;
            shortest = adj_coord;
        }
    }

    return shortest;
}
