#ifndef COORDINATES_H
#define COORDINATES_H

#ifndef SQUARE
#define SQUARE(X) ((X) * (X))
#endif // !SQUARE

#include <math.h>
#include <stdlib.h>

static const int DIR_X[5] = {0, 0, -1, 1, 0};
static const int DIR_Y[5] = {-1, 1, 0, 0, 0};

typedef enum direction {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
    IGNORE
} direction;

typedef struct coordinates {
    int X;
    int Y;
} coordinates;

static inline int compare_coordinates(coordinates coord_A, coordinates coord_B) {
    return coord_A.X == coord_B.X && coord_A.Y == coord_B.Y;
}

static inline coordinates coordinates_mult(coordinates coord_A, int times) {
    return (coordinates){coord_A.X * times, coord_A.Y * times};
}

static inline coordinates coordinates_prod(coordinates coord_A, coordinates coord_B) {
    return (coordinates){coord_A.X * coord_B.X, coord_A.Y * coord_B.Y};
}

static inline coordinates coordinates_sum(coordinates coord_A, coordinates coord_B) {
    return (coordinates){coord_A.X + coord_B.X, coord_A.Y + coord_B.Y};
}

static inline coordinates coordinates_diff(coordinates coord_A, coordinates coord_B) {
    return (coordinates){coord_A.X - coord_B.X, coord_A.Y - coord_B.Y};
}

static inline float euclidean_coordinate_distance_to(coordinates coord_A, coordinates coord_B) {
    return sqrt(SQUARE(coord_A.X - coord_B.X) + SQUARE(coord_A.Y - coord_B.Y));
}

static inline int manhattan_coordinate_distance_to(coordinates coord_A, coordinates coord_B) {
    return abs(coord_A.X - coord_B.X) + abs(coord_A.Y - coord_B.Y);
}

static inline direction get_direction_from_input(char c) {
    switch (c) {
        case 'A':
            return UP;

        case 'D':
            return LEFT;

        case 'B':
            return DOWN;

        case 'C':
            return RIGHT;

        default: return IGNORE;
    }
}

#endif // !COORDINATES_H
