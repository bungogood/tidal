#pragma once

#define empty_fen "8/8/8/8/8/8/8/8 w - - 0 1"
#define start_fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define tricky_fen "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_fen "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_fen "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9"

typedef unsigned long long U64;

enum Square {
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8, none
};

extern const char* player_str[];
extern const char* sq_str[];
extern const char* ascii_piece;

enum Player   { black, white, both };
enum Castling { wk = 8, wq = 4, bk = 2, bq = 1, no_castle = 0 };
enum Piece    { P, N, B, R, Q, K, p, n, b, r, q, k };

typedef struct Moves {
    int moves[256];
    int count;
} Moves;

typedef struct Board {
    Moves moves;
    U64 bb[12]; // piece bitboards
    U64 occ[3];
    int isWhite;
    int enPassant;
    int castling;  // KQkq
    int halfMove;
    int fullMove;
} Board;

void to_fen(char* fen, const Board* board);
void from_fen(const char* fen, Board* board);
Board* create_from_fen(const char* fen);
void to_san(char* san, int move, const Board* board);
int from_san(const char* san, const Board* board);

const void print_bb(const U64 bb);
const void print_board(const Board* board);
const int chr_piece(const char chr);
const int str_square(const char* str);
const int get_piece(const int sq, const Board* board);
