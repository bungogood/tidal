#include <stdio.h>
#include <stdlib.h>
#include <test.h>
#include <bitboard.h>

// some form of flags handler

int main(int argc, char *argv[]) {
    init_attacks();
    char fen[100];
    // Board* board = create_from_fen(start_fen);
    // Board* board = create_from_fen(tricky_fen);
    // Board* board = create_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    // to_fen(fen, board);
    // printf("fen: %s\n", fen);

    long expected[] = { 48, 2039, 97862, 4085603, 193690690, 8031647685 };
    int maxDepth = 6;
    perftest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0", maxDepth, expected);
     
    return EXIT_SUCCESS;
}