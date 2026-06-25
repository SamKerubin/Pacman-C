#include "board.h"
#include "coordinates.h"
#include "input.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;

static void finish(int signo) {
    (void)signo;

    printf("Quitting game...\n");
    running = 0;
}

static void setup_signal() {
    struct sigaction sig;
    sig.sa_handler = finish;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    sigaction(SIGINT, &sig, NULL);
}

static void clear_screen() {
    system("clear");
}

int main() {
    setup_signal();
    set_conio_terminal_mode();
    clear_screen();

    board *board = init_board();

    print_board(board);
    update_board(board);

    while (running) {
        if (board->lifes <= 0) {
            finish(SIGINT);
            continue;
        }

        char c = '\0';

        read(STDIN_FILENO, &c, 1);
        if (c == '\033') {
            read(STDIN_FILENO, &c, 1);
            read(STDIN_FILENO, &c, 1);

            direction d = get_direction_from_input(c);
            if (d != IGNORE) {
                set_pacman_direction(board, d);
            }
        }

        direction d = get_pacman_direction(board);
        if (!move_pacman(board, d)) {
            life_lost(board);
        }

        clear_screen();
        update_ghosts(board);
        update_board(board);
        print_board(board);
        usleep(1200);
    }

    reset_terminal_mode();
    end_game(board);
    return 0;
}
