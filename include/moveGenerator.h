#pragma once

#include <chess.h>
#include <bitboard.h>

/*
    binary move bits                                   shift  hexidecimal constants
    
    0000 0000 0000 0000 0000 0011 1111  source square     0   0x3f
    0000 0000 0000 0000 1111 1100 0000  target square     6   0xfc0
    0000 0000 0000 1111 0000 0000 0000  piece             12  0xf000
    0000 0000 1111 0000 0000 0000 0000  promoted piece    16  0xf0000
    0000 1111 0000 0000 0000 0000 0000  captured piece    20  0xf00000
    0001 0000 0000 0000 0000 0000 0000  capture flag      24  0x1000000
    0010 0000 0000 0000 0000 0000 0000  double push flag  25  0x2000000
    0100 0000 0000 0000 0000 0000 0000  enpassant flag    26  0x4000000
    1000 0000 0000 0000 0000 0000 0000  castling flag     27  0x8000000
*/

#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 24) |   \
    (double << 25) |    \
    (enpassant << 26) | \
    (castling << 27) \

#define create_quite(source, target, piece) \
    (source) |          \
    (target << 6) |     \
    (piece << 12)     \

#define create_capture(source, target, piece, captured) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (captured << 20) |   \
    (1 << 24) \

#define create_enPassant(source, target, piece, captured) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (captured << 20) |   \
    (1 << 24) | \
    (1 << 26) \

#define create_promtion_capture(source, target, piece, promoted, captured) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (captured << 20) |   \
    (1 << 24) \

#define create_double(source, target, piece) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (1 << 25) \

#define create_promtion(source, target, piece, promoted) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) \

#define create_castle(source, target, piece) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (1 << 27) \

#define get_move_source(move) (move & 0x3f)
#define get_move_target(move) ((move & 0xfc0) >> 6)
#define get_move_piece(move) ((move & 0xf000) >> 12)
#define get_move_promoted(move) ((move & 0xf0000) >> 16)
#define get_move_captured(move) ((move & 0xf00000) >> 20)
#define get_move_capture(move) (move & 0x1000000)
#define get_move_double(move) (move & 0x2000000)
#define get_move_enpassant(move) (move & 0x4000000)
#define get_move_castling(move) (move & 0x8000000)

extern Moves moves;

const void print_attacked(Board* board, int isWhite);

void generate_moves(Board* board);
void generate_legal(Board* board);
void generate_captures(Board* board);

void print_move(int move);
void print_san(int move, Board* board);
void print_moves(Board* board);
