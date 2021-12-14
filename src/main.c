#include <stdio.h>
#include <chess.h>
#include <bitboard.h>

// some form of flags handler

int main(int argc, char *argv[]) {
    init_attacks();
    Board* board = from_fen(start_fen);
    print_board(board);
    return 0;
}
