#include "pathfinding.h"
#include "board.h"
#include "coordinates.h"
#include <math.h>

coordinates find_shortest_path(uint8_t id, uint8_t **board,
                               coordinates start,
                               coordinates end) {
    if (compare_coordinates(start, end)) {
        return start;
    }

    coordinates shortest = {-1, -1};
    float shortest_distance = INFINITY;
    for (int i = 0; i < 4; i++) {
        coordinates adj_coord = (coordinates){start.X + DIR_X[i], start.Y + DIR_Y[i]};

        if (!is_valid(adj_coord, board, id)) {
            continue;
        }

        float euclidean_distance = coordinate_distance_to(adj_coord, end);
        if (euclidean_distance < shortest_distance) {
            shortest_distance = euclidean_distance;
            shortest = adj_coord;
        }
    }

    return shortest;
}
