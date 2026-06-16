#include "board.h"
#include "board_data.h"
#include "coordinates.h"
#include "ghost.h"
#include "pacman.h"
#include <stdio.h>
#include <stdlib.h>

board *init_board() {
    board *b = (board *)calloc(1, sizeof(board));

    b->board = (uint8_t **)malloc(BOARD_HEIGHT * sizeof(uint8_t *));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        b->board[i] = (uint8_t *)malloc(BOARD_WIDTH * sizeof(uint8_t));
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (BOARD[i][j] == DOT_ID && b->remaining_dots < TOTAL_DOTS) {
                b->dots_positions[b->remaining_dots++] = (coordinates){i, j};
            }

            if (BOARD[i][j] == POWER_DOT_ID && b->remaining_power_dots < TOTAL_POWER_DOTS) {
                b->power_dots_positions[b->remaining_power_dots++] = (coordinates){i, j};
            }

            b->board[i][j] = BOARD[i][j];
        }
    }

    b->lifes = STARTER_LIFES;

    b->blinky = init_ghost(BLINKY_ID, BLINKY_INIT_POSITION);
    b->blinky->state = SCATTER;

    b->board[b->blinky->position.Y][b->blinky->position.X] = b->blinky->id;

    b->pinky = init_ghost(PINKY_ID, PINKY_INIT_POSITION);
    b->inky = init_ghost(INKY_ID, INKY_INIT_POSITION);
    b->clyde = init_ghost(CLYDE_ID, CLYDE_INIT_POSITION);

    b->board[b->pinky->position.Y][b->pinky->position.X] = b->pinky->id;
    b->board[b->inky->position.Y][b->inky->position.X] = b->inky->id;
    b->board[b->clyde->position.Y][b->clyde->position.X] = b->clyde->id;

    b->pacman = init_pacman(PACMAN_ID, PACMAN_INIT_POSITION);
    b->board[b->pacman->position.Y][b->pacman->position.X] = b->pacman->id;

    return b;
}

void end_game(board *board) {
    if (!board) {
        return;
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        free(board->board[i]);
    }
    free(board->board);

    free_ghost(board->blinky);
    free_ghost(board->pinky);
    free_ghost(board->inky);
    free_ghost(board->clyde);

    free_pacman(board->pacman);

    free(board);
}


int is_inside_bounds(coordinates coord) {
    return (coord.X < BOARD_HEIGHT && coord.X >= 0)
            && (coord.Y < BOARD_WIDTH && coord.Y >= 0);
}

static inline int is_wall(coordinates coord, uint8_t **board) {
    return board[coord.Y][coord.X] == WALL_ID;
}

static inline int is_ghost(coordinates coord, uint8_t **board) {
    return board[coord.Y][coord.X] >= BLINKY_ID && board[coord.Y][coord.X] <= CLYDE_ID;
}

int is_valid(coordinates coord,
                    uint8_t **board,
                    uint8_t id) {
    return is_inside_bounds(coord)
            && !is_wall(coord, board)
            && board[coord.Y][coord.X] != id;
}

void update_board(board *board) {
    ghost *blinky = board->blinky;
    ghost *pinky = board->pinky;
    ghost *inky = board->inky;
    ghost *clyde = board->clyde;

    board->board[blinky->last_position.Y][blinky->last_position.X] = EMPTY_ID;
    board->board[pinky->last_position.Y][pinky->last_position.X] = EMPTY_ID;
    board->board[inky->last_position.Y][inky->last_position.X] = EMPTY_ID;
    board->board[clyde->last_position.Y][clyde->last_position.X] = EMPTY_ID;

    board->board[blinky->position.Y][blinky->position.X] = blinky->id;
    board->board[pinky->position.Y][pinky->position.X] = pinky->id;
    board->board[inky->position.Y][inky->position.X] = inky->id;
    board->board[clyde->position.Y][clyde->position.X] = clyde->id;

    pacman *pacman = board->pacman;

    board->board[pacman->last_position.Y][pacman->last_position.X] = EMPTY_ID;
    board->board[pacman->position.Y][pacman->position.X] = pacman->id;

    for(int i = 0; i < board->remaining_dots; i++) {
        coordinates curr_dot = board->dots_positions[i];
        board->board[curr_dot.Y][curr_dot.X] = DOT_ID;
    }

    for (int i = 0; i < board->remaining_power_dots; i++) {
        coordinates curr_pow = board->power_dots_positions[i];
        board->board[curr_pow.Y][curr_pow.X] = POWER_DOT_ID;
    }
}

static void print_pacman(direction pacman_dir) {
    switch (pacman_dir) {
        case UP: printf(BLACK_BACKGROUND YELLOW PACMAN_ICON_UP " " RESET); break;
        case DOWN: printf(BLACK_BACKGROUND YELLOW PACMAN_ICON_DOWN " " RESET); break;
        case LEFT: printf(BLACK_BACKGROUND YELLOW PACMAN_ICON_LEFT " " RESET); break;
        case RIGHT: printf(BLACK_BACKGROUND YELLOW PACMAN_ICON_RIGHT " " RESET); break;
        default: break;
    }
}

void print_board(board *board) {
    printf(BLACK_BACKGROUND "   1UP      HIGH   SCORE                                \n" RESET);
    printf(BLACK_BACKGROUND "     %d                                                  " RESET "\n", board->score);
    printf(BLACK_BACKGROUND "                                                        " RESET "\n");

    for (int i = 3; i < BOARD_HEIGHT - 2; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            uint8_t tile = board->board[i][j];

            switch (tile) {
                case WALL_ID: printf(BLACK_BACKGROUND BLUE WALL_ICON " " RESET); break;
                case HOME_DOOR_ID: printf(BLACK_BACKGROUND PINK HOME_DOOR_ICON " " RESET); break;
                case EMPTY_ID: printf(BLACK_BACKGROUND "  " RESET); break;
                case DOT_ID: printf(BLACK_BACKGROUND LIGHT_YELLOW DOT_ICON " " RESET); break;
                case POWER_DOT_ID: printf(BLACK_BACKGROUND LIGHT_YELLOW POWER_DOT_ICON " " RESET); break;
                case PACMAN_ID: print_pacman(board->pacman->current_direction); break;
                case BLINKY_ID: printf(BLACK_BACKGROUND RED GHOST_ICON " " RESET); break;
                case PINKY_ID: printf(BLACK_BACKGROUND PINK GHOST_ICON " " RESET); break;
                case INKY_ID: printf(BLACK_BACKGROUND CYAN GHOST_ICON " " RESET); break;
                case CLYDE_ID: printf(BLACK_BACKGROUND ORANGE GHOST_ICON " " RESET); break;
                default: break;
            }

            if (j == BOARD_WIDTH - 1) {
                printf("\n");
            }
        }
    }

    printf(BLACK_BACKGROUND "                                                        " RESET "\n");
    for (int i = 0; i < board->lifes; i++) {
        printf(BLACK_BACKGROUND YELLOW LIFE_ICON " " RESET);
    }
    printf(BLACK_BACKGROUND "                                                  \n" RESET);
}

int move_pacman(board *board, direction d) {
    pacman *pacman = board->pacman;

    direction new_direction = d;

    coordinates direction = {DIR_X[d], DIR_Y[d]};
    coordinates pacman_new_pos = coordinates_sum(pacman->position, direction);
    if (is_wall(pacman_new_pos, board->board)) {
        new_direction = pacman->last_direction;
    }

    if (!is_inside_bounds(pacman_new_pos)) {
        pacman_new_pos.X = (pacman_new_pos.X % BOARD_HEIGHT + BOARD_HEIGHT) % BOARD_HEIGHT;
        pacman_new_pos.Y = (pacman_new_pos.Y % BOARD_WIDTH + BOARD_WIDTH) % BOARD_WIDTH;
    }

    pacman->last_direction = pacman->current_direction;
    pacman->current_direction = new_direction;
    return 1;
}

void move_ghosts(board *board) {
    move_ghost(board->blinky, board->board, get_blinky_target_position(board));
    move_ghost(board->pinky, board->board, get_pinky_target_position(board));
    move_ghost(board->inky, board->board, get_inky_target_position(board));
    move_ghost(board->clyde, board->board, get_clyde_target_position(board));
}

void eat_ghost(board *board) {
    (void)board;
}

void eat_dot(board *board) {
    board->remaining_dots--;
    board->preferred_ghost->dot_counter++;
    board->score += SCORE_PER_DOT;
}

void eat_power_dot(board *board) {
    board->remaining_dots--;
    board->preferred_ghost->dot_counter++;
    board->score += SCORE_PER_DOT;
}

static coordinates get_random_valid_adjacent(coordinates start, uint8_t **board, uint8_t id) {
    coordinates adj[4] = {0};
    for (int i = 0; i < 4; i++) {
        adj[i] = (coordinates){start.X + DIR_X[i], start.Y + DIR_Y[i]};
    }

    int r = -1;
    do {
        r = rand() % 4;
    } while(!is_valid(adj[r], board, id));

    return adj[r];
}

coordinates get_blinky_target_position(board *board) {
    switch (board->blinky->state) {
        case SCATTER: return BLINKY_SCATTER_TARGET;
        case CHASE: return board->pacman->position;
        case FRIGHTENED: return get_random_valid_adjacent(board->blinky->position,
                                                          board->board,
                                                          board->blinky->id); 
        case EATEN: return HOME_POSITION;
        case INIT:
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates pinky_chase_target(board *board) {
    const coordinates OFFSET = {4, 4};
 
    coordinates pacman_pos = board->pacman->position;
    direction pacman_dir = board->pacman->current_direction;

    coordinates direction = (coordinates){DIR_X[pacman_dir], DIR_Y[pacman_dir]};
    coordinates prod = coordinates_prod(OFFSET, direction);

    return coordinates_sum(pacman_pos, prod);
}

coordinates get_pinky_target_position(board *board) {
    switch (board->pinky->state) {
        case INIT: return board->pinky->position;
        case SCATTER: return PINKY_SCATTER_TARGET;
        case CHASE: return pinky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->pinky->position,
                                                          board->board,
                                                          board->pinky->id); 
        case EATEN: return HOME_POSITION;
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates inky_chase_target(board *board) {
    const coordinates OFFSET = {2, 2};

    coordinates pacman_pos = board->pacman->position;
    direction pacman_dir = board->pacman->current_direction;

    coordinates direction = {DIR_X[pacman_dir], DIR_Y[pacman_dir]};
    coordinates prod = coordinates_prod(OFFSET, direction);

    coordinates pacman_offset = coordinates_sum(pacman_pos, prod);

    coordinates blinky_pos = board->blinky->position;

    coordinates diff = coordinates_diff(blinky_pos, pacman_offset);
    coordinates mult = coordinates_mult(diff, 2);

    return coordinates_sum(mult, blinky_pos);
}

coordinates get_inky_target_position(board *board) {
    switch (board->inky->state) {
        case INIT: return board->inky->position;
        case SCATTER: return INKY_SCATTER_TARGET;
        case CHASE: return inky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->inky->position,
                                                          board->board,
                                                          board->inky->id); 
        case EATEN: return HOME_POSITION;
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates clyde_chase_target(board *board) {
    if (coordinate_distance_to(board->pacman->position, board->clyde->position) > 8.0f) {
        return board->pacman->position;
    }

    return CLYDE_SCATTER_TARGET;

}

coordinates get_clyde_target_position(board *board) {
    switch (board->clyde->state) {
        case INIT: return board->clyde->position;
        case SCATTER: return CLYDE_SCATTER_TARGET;
        case CHASE: return clyde_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->clyde->position,
                                                          board->board,
                                                          board->clyde->id); 
        case EATEN: return HOME_POSITION;
        default:
            return (coordinates){-1, -1};

    }
}

void life_lost(board *board) {
    board->lifes--;
}

void go_next_level(board *board) {
    board->level++;
}

