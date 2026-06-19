#include "board.h"
#include <stdlib.h>
#include <unistd.h>

int main() {
    board *board = init_board();

    system("clear");
    print_board(board);

    move_pacman(board, LEFT);
    update_board(board);

    while (1) {
        move_ghosts(board);
        usleep(1000000);
        update_ghosts(board);
        update_board(board);
        system("clear");
        print_board(board);
    }

    end_game(board);
    return 0;
}
