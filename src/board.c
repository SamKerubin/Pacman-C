#include "board.h"
#include "board_data.h"
#include "coordinates.h"
#include "ghost.h"
#include "time_ms.h"
#include "pacman.h"
#include <stdio.h>
#include <stdlib.h>

static void fill_board(board *board) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (BOARD[i][j] == DOT_ID && board->remaining_dots < TOTAL_DOTS) {
                board->dots_positions[board->remaining_dots++] = (coordinates){j, i};
            }

            if (BOARD[i][j] == POWER_DOT_ID && board->remaining_power_dots < TOTAL_POWER_DOTS) {
                board->power_dots_positions[board->remaining_power_dots++] = (coordinates){j, i};
            }

            board->board[i][j] = BOARD[i][j];
        }
    }
}

board *init_board() {
    board *b = (board *)calloc(1, sizeof(board));

    b->board = (uint8_t **)malloc(BOARD_HEIGHT * sizeof(uint8_t *));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        b->board[i] = (uint8_t *)malloc(BOARD_WIDTH * sizeof(uint8_t));
    }

    fill_board(b);

    b->lifes = STARTER_LIFES;

    b->blinky = init_ghost(BLINKY_ID, BLINKY_INIT_POSITION);
    b->blinky->state = SCATTER;
    b->blinky->scatter_time = get_time_ms() + GHOST_SCATTER_TIME_MS;

    b->board[b->blinky->position.Y][b->blinky->position.X] = b->blinky->id;

    b->pinky = init_ghost(PINKY_ID, PINKY_INIT_POSITION);
    b->inky = init_ghost(INKY_ID, INKY_INIT_POSITION);
    b->clyde = init_ghost(CLYDE_ID, CLYDE_INIT_POSITION);

    b->pinky->dot_limit = PINKY_DOT_LIMIT;
    b->inky->dot_limit = INKY_DOT_LIMIT;
    b->clyde->dot_limit = CLYDE_DOT_LIMIT;

    b->preferred_ghost[0] = b->pinky;
    b->preferred_ghost[1] = b->inky;
    b->preferred_ghost[2] = b->clyde;

    b->current_counter_reference = &b->preferred_ghost[0]->dot_counter;

    b->board[b->pinky->position.Y][b->pinky->position.X] = b->pinky->id;
    b->board[b->inky->position.Y][b->inky->position.X] = b->inky->id;
    b->board[b->clyde->position.Y][b->clyde->position.X] = b->clyde->id;

    b->pacman = init_pacman(PACMAN_ID, PACMAN_INIT_POSITION);
    b->board[b->pacman->position.Y][b->pacman->position.X] = b->pacman->id;

    b->last_dot_eaten_time = get_time_ms() + PREFERRED_GHOST_AUTOMATIC_MOVE_TIME_MS;

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

int is_wall(coordinates coord, uint8_t **board) {
    return board[coord.Y][coord.X] == WALL_ID;
}

static inline int is_dot(coordinates coord, uint8_t **board) {
    return board[coord.Y][coord.X] == DOT_ID;
}

static inline int is_power_dot(coordinates coord, uint8_t **board) {
    return board[coord.Y][coord.X] == POWER_DOT_ID;
}

static inline int is_ghost(coordinates coord, uint8_t **board) {
    uint8_t tile = board[coord.Y][coord.X];
    return tile == BLINKY_ID
           || tile == PINKY_ID
           || tile == INKY_ID
           || tile == CLYDE_ID;
}

static void update_ghost_state(ghost *ghost, uint8_t has_eaten_power_dot) {
    // TODO: Invert ghost position when changed state
    int64_t now = get_time_ms();
    ghost_state state = ghost->state;

    switch (state) {
        case OUT_OF_HOUSE:
            if (compare_coordinates(ghost->position, OUT_HOME_LEFT_POSITION)
                || compare_coordinates(ghost->position, OUT_HOME_RIGHT_POSITION)) {
                ghost->state = SCATTER;
                ghost->scatter_time = get_time_ms() + GHOST_SCATTER_TIME_MS;
            }
        break;

        case EATEN:
            if (compare_coordinates(ghost->position, IN_HOME_POSITION)) {
                ghost->state = OUT_OF_HOUSE;
            }
        break;

        case SCATTER:
            if (now >= ghost->scatter_time) {
                ghost->state = CHASE;
                ghost->chase_time = now + GHOST_CHASE_TIME_MS;
            }
        break;

        case CHASE:
            if (now >= ghost->chase_time) {
                ghost->state = SCATTER;
                ghost->scatter_time = now + GHOST_SCATTER_TIME_MS;
            }
        break;

        case FRIGHTENED:
            if (!has_eaten_power_dot) {
                ghost->state = SCATTER;
                ghost->scatter_time = now + GHOST_SCATTER_TIME_MS;
            }
        break;

        default: return;
    }
}

void update_ghosts(board *board) {
    ghost *blinky = board->blinky;
    ghost *pinky = board->pinky;
    ghost *inky = board->inky;
    ghost *clyde = board->clyde;

    ghost *preferred = NULL;
    if (board->current_ghost < 3) {
        preferred = board->preferred_ghost[board->current_ghost];
    }

    if (preferred) {
        if (*board->current_counter_reference >= preferred->dot_limit) {
            preferred->state = OUT_OF_HOUSE;
            board->current_ghost++;
            if (board->current_ghost < 3) {
                board->current_counter_reference = &board->preferred_ghost[board->current_ghost]->dot_counter;
            }
        }

        int64_t now = get_time_ms();
        if (now >= board->last_dot_eaten_time) {
            board->last_dot_eaten_time = get_time_ms() + PREFERRED_GHOST_AUTOMATIC_MOVE_TIME_MS;
            preferred->state = OUT_OF_HOUSE;
            board->current_ghost++;
            if (board->current_ghost < 3) {
                board->current_counter_reference = &board->preferred_ghost[board->current_ghost]->dot_counter;
            }
        }
    }

    update_ghost_state(blinky, board->has_eaten_power_dot);
    update_ghost_state(pinky, board->has_eaten_power_dot);
    update_ghost_state(inky, board->has_eaten_power_dot);
    update_ghost_state(clyde, board->has_eaten_power_dot);

    move_ghost(blinky, board->board, get_blinky_target_position(board));
    move_ghost(pinky, board->board, get_pinky_target_position(board));
    move_ghost(inky, board->board, get_inky_target_position(board));
    move_ghost(clyde, board->board, get_clyde_target_position(board));
}

void update_board(board *board) {
    if (board->score > 0 && board->score % 10000 == 0) {
        board->lifes++;
    }

    if (board->has_eaten_power_dot) {
        int64_t now = get_time_ms();
        if (now >= board->power_dot_time) {
            board->power_dot_time = 0;
            board->has_eaten_power_dot = 0;
        }
    }

    pacman *pacman = board->pacman;

    ghost *blinky = board->blinky;
    ghost *pinky = board->pinky;
    ghost *inky = board->inky;
    ghost *clyde = board->clyde;

    board->board[pacman->last_position.Y][pacman->last_position.X] = EMPTY_ID;

    board->board[blinky->last_position.Y][blinky->last_position.X] = EMPTY_ID;
    board->board[pinky->last_position.Y][pinky->last_position.X] = EMPTY_ID;
    board->board[inky->last_position.Y][inky->last_position.X] = EMPTY_ID;
    board->board[clyde->last_position.Y][clyde->last_position.X] = EMPTY_ID;

    board->board[BOARD_HEIGHT / 2 - 3][BOARD_WIDTH / 2 - 1] = HOME_DOOR_ID;
    board->board[BOARD_HEIGHT / 2 - 3][BOARD_WIDTH / 2] = HOME_DOOR_ID;

    for(int i = 0; i < board->remaining_dots; i++) {
        coordinates curr_dot = board->dots_positions[i];
        board->board[curr_dot.Y][curr_dot.X] = DOT_ID;
    }

    for (int i = 0; i < board->remaining_power_dots; i++) {
        coordinates curr_pow = board->power_dots_positions[i];
        board->board[curr_pow.Y][curr_pow.X] = POWER_DOT_ID;
    }

    board->board[pacman->position.Y][pacman->position.X] = pacman->id;

    board->board[blinky->position.Y][blinky->position.X] = blinky->id;
    board->board[pinky->position.Y][pinky->position.X] = pinky->id;
    board->board[inky->position.Y][inky->position.X] = inky->id;
    board->board[clyde->position.Y][clyde->position.X] = clyde->id;
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
    printf(BLACK_BACKGROUND "                                                  " RESET "\n");
}

int move_pacman(board *board, direction d) {
    if (get_time_ms() < board->pacman->next_movement_time) {
        return 1;
    }

    pacman *pacman = board->pacman;
    pacman->current_direction = d;

    pacman->next_movement_time = get_time_ms() + PACMAN_MOVEMENT_TIME_MS;

    coordinates direction = {DIR_Y[d], DIR_X[d]};
    coordinates pacman_new_pos = coordinates_sum(pacman->position, direction);

    if (is_wall(pacman_new_pos, board->board)
        || board->board[pacman_new_pos.Y][pacman_new_pos.X] == HOME_DOOR_ID) {
        return 1;
    }

    if (!is_inside_bounds(pacman_new_pos)) {
        pacman_new_pos.X = (pacman_new_pos.X % BOARD_HEIGHT + BOARD_HEIGHT) % BOARD_HEIGHT;
        pacman_new_pos.Y = (pacman_new_pos.Y % BOARD_WIDTH + BOARD_WIDTH) % BOARD_WIDTH;
    }

    if (is_ghost(pacman_new_pos, board->board)) {
        if (!board->has_eaten_power_dot) {
            return 0;
        }

        entity_id ghost_id = board->board[pacman_new_pos.Y][pacman_new_pos.X];
        eat_ghost(board, ghost_id);
    }

    if (is_dot(pacman_new_pos, board->board)) {
        eat_dot(pacman_new_pos, board);
    }

    if (is_power_dot(pacman_new_pos, board->board)) {
        eat_power_dot(pacman_new_pos, board);
    }

    pacman->last_position = pacman->position;
    pacman->position = pacman_new_pos;

    return 1;
}

void eat_ghost(board *board, entity_id ghost_id) {
    ghost *ghost = NULL;
    switch (ghost_id) {
        case BLINKY_ID: ghost = board->blinky; break;
        case PINKY_ID: ghost = board->pinky; break;
        case INKY_ID: ghost = board->inky; break;
        case CLYDE_ID: ghost = board->clyde; break;
        default: break;
    }

    ghost->state = EATEN;
    board->score += BASE_GHOST_EATEN_SCORE << board->ghosts_eaten_count++;
}

void eat_dot(coordinates dot_pos, board *board) {
    for (int i = 0; i < board->remaining_dots; i++) {
        coordinates curr_dot = board->dots_positions[i];
        if (compare_coordinates(curr_dot, dot_pos)) {
            coordinates last = board->dots_positions[board->remaining_dots - 1];
            board->dots_positions[board->remaining_dots - 1] = curr_dot;
            board->dots_positions[i] = last;
            break;
        }
    }

    board->remaining_dots--;

    if (board->current_ghost < 3) {
        (*board->current_counter_reference)++;
        board->last_dot_eaten_time = get_time_ms() + PREFERRED_GHOST_AUTOMATIC_MOVE_TIME_MS;
    }

    board->score += SCORE_PER_DOT;
}

static void make_ghosts_frightened(board *board) {
    ghost *blinky = board->blinky;
    ghost *pinky = board->pinky;
    ghost *inky = board->inky;
    ghost *clyde = board->clyde;

    blinky->state = FRIGHTENED;
    pinky->state = FRIGHTENED;
    inky->state = FRIGHTENED;
    clyde->state = FRIGHTENED;

    // TODO: Handle movement invertion of the ghost when frightened
}

void eat_power_dot(coordinates pow_pos, board *board) {
    for (int i = 0; i < board->remaining_power_dots; i++) {
        coordinates curr_pow = board->power_dots_positions[i];
        if (compare_coordinates(curr_pow, pow_pos)) {
            coordinates last = board->power_dots_positions[board->remaining_power_dots - 1];
            board->power_dots_positions[board->remaining_power_dots - 1] = curr_pow;
            board->power_dots_positions[i] = last;
            break;
        }
    }

    board->remaining_power_dots--;
    board->ghosts_eaten_count = 0;

    if (board->current_ghost < 3) {
        (*board->current_counter_reference)++;
        board->last_dot_eaten_time = get_time_ms() + PREFERRED_GHOST_AUTOMATIC_MOVE_TIME_MS;
    }

    board->has_eaten_power_dot = 1;
    board->power_dot_time = get_time_ms() + FRIGHTENED_TIME_MS;

    board->score += SCORE_PER_DOT;

    make_ghosts_frightened(board);
}

static coordinates get_init_ghost_target(coordinates initial_coord, ghost *ghost) {
    if (compare_coordinates(initial_coord, ghost->position)) {
        return coordinates_sum(initial_coord, (coordinates){DIR_Y[ghost->current_direction], DIR_X[ghost->current_direction]});
    }

    if (ghost->current_direction == UP) {
        return coordinates_sum(initial_coord, (coordinates){DIR_Y[DOWN], DIR_X[DOWN]});
    }

    return coordinates_sum(initial_coord, (coordinates){DIR_Y[UP], DIR_X[UP]});
}

static coordinates get_random_valid_adjacent(coordinates start, uint8_t **board, uint8_t id, ghost_state state) {
    coordinates adj[4] = {0};
    for (int i = 0; i < 4; i++) {
        adj[i] = (coordinates){start.X + DIR_X[i], start.Y + DIR_Y[i]};
    }

    int r = -1;
    do {
        r = rand() % 4;
    } while(!is_valid_ghost_movement(adj[r], board, id, state));

    return adj[r];
}

coordinates get_blinky_target_position(board *board) {
    switch (board->blinky->state) {
        case OUT_OF_HOUSE: return OUT_HOME_LEFT_POSITION;
        case SCATTER: return BLINKY_SCATTER_TARGET;
        case CHASE: return board->pacman->position;
        case FRIGHTENED: return get_random_valid_adjacent(board->blinky->position,
                                                          board->board,
                                                          board->blinky->id,
                                                          board->blinky->state);
        case EATEN: return IN_HOME_POSITION;
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
        case INIT: return get_init_ghost_target(PINKY_INIT_POSITION, board->pinky);
        case OUT_OF_HOUSE: return OUT_HOME_LEFT_POSITION;
        case SCATTER: return PINKY_SCATTER_TARGET;
        case CHASE: return pinky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->pinky->position,
                                                          board->board,
                                                          board->pinky->id,
                                                          board->pinky->state);
        case EATEN: return IN_HOME_POSITION;
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

    coordinates diff = coordinates_diff(pacman_offset, blinky_pos);
    coordinates mult = coordinates_mult(diff, 2);

    return coordinates_sum(mult, blinky_pos);
}

coordinates get_inky_target_position(board *board) {
    switch (board->inky->state) {
        case INIT: return get_init_ghost_target(INKY_INIT_POSITION, board->inky);
        case OUT_OF_HOUSE: return OUT_HOME_LEFT_POSITION;
        case SCATTER: return INKY_SCATTER_TARGET;
        case CHASE: return inky_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->inky->position,
                                                          board->board,
                                                          board->inky->id,
                                                          board->inky->state);
        case EATEN: return IN_HOME_POSITION;
        default:
            return (coordinates){-1, -1};
    }
}

static coordinates clyde_chase_target(board *board) {
    if (euclidean_coordinate_distance_to(board->pacman->position, board->clyde->position) > 8.0f) {
        return board->pacman->position;
    }

    return CLYDE_SCATTER_TARGET;
}

coordinates get_clyde_target_position(board *board) {
    switch (board->clyde->state) {
        case INIT: return get_init_ghost_target(CLYDE_INIT_POSITION, board->clyde);
        case OUT_OF_HOUSE: return OUT_HOME_LEFT_POSITION;
        case SCATTER: return CLYDE_SCATTER_TARGET;
        case CHASE: return clyde_chase_target(board);
        case FRIGHTENED: return get_random_valid_adjacent(board->clyde->position,
                                                          board->board,
                                                          board->clyde->id,
                                                          board->clyde->state);
        case EATEN: return IN_HOME_POSITION;
        default:
            return (coordinates){-1, -1};
    }
}

void life_lost(board *board) {
    board->lifes--;
}

void go_next_level(board *board) {
    board->level++;
    fill_board(board);

    board->blinky->position = BLINKY_INIT_POSITION;
    board->pinky->position = PINKY_INIT_POSITION;
    board->inky->position = INKY_INIT_POSITION;
    board->clyde->position = CLYDE_INIT_POSITION;

    board->pacman->position = PACMAN_INIT_POSITION;

    int64_t now = get_time_ms();

    board->blinky->next_movement_time = now;
    board->pinky->next_movement_time = now;
    board->inky->next_movement_time = now;
    board->clyde->next_movement_time = now;

    board->pacman->next_movement_time = now;

    board->blinky->scatter_time = now + GHOST_SCATTER_TIME_MS;

}

