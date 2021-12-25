#include <chess.h>
#include <moveGenerator.h>
#include <string.h>
#include <stdlib.h>

const int castlingUpdate[] = { // KQkq
    11, 15, 15, 15,  3, 15, 15,  7,
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    14, 15, 15, 15, 12, 15, 15, 13, 
};

// update occ
void make_move(Board* board, int move) {
    int src       = get_move_source(move);
    int dest      = get_move_target(move);
    int piece     = get_move_piece(move);
    int promotion = get_move_promoted(move);
    int capture   = get_move_capture(move);
    int doubles   = get_move_double(move);
    int castles   = get_move_castling(move);
    int enPassant = get_move_enpassant(move);
    U64* pocc = &board->occ[board->isWhite];

    pop_bit(src,  board->bb[piece]);
    if (promotion) set_bit(dest, board->bb[promotion]);
    else set_bit(dest, board->bb[piece]);

    pop_bit(src,  *pocc);
    set_bit(dest, *pocc);

    if (capture) {
        U64* eocc = &board->occ[!board->isWhite];
        if (enPassant) {
            if (board->isWhite) {
                pop_bit(dest-8, board->bb[p]);
                pop_bit(dest-8, *eocc);
            } else {
                pop_bit(dest+8, board->bb[P]);
                pop_bit(dest+8, *eocc);
            }
        } else {
            int captured = get_move_captured(move);
            pop_bit(dest, board->bb[captured]);
            pop_bit(dest, *eocc);
        }
    }

    if (doubles) board->enPassant = board->isWhite ? dest-8 : dest+8;
    else  board->enPassant = none;

    if (castles) {
        switch (dest) {
            case c1:
                pop_bit(a1, board->bb[R]);
                pop_bit(a1, *pocc);
                set_bit(d1, board->bb[R]);
                set_bit(d1, *pocc);
                break;
            case g1:
                pop_bit(h1, board->bb[R]);
                pop_bit(h1, *pocc);
                set_bit(f1, board->bb[R]);
                set_bit(f1, *pocc);
                break;
            case c8:
                pop_bit(a8, board->bb[r]);
                pop_bit(a8, *pocc);
                set_bit(d8, board->bb[r]);
                set_bit(d8, *pocc);
                break;
            case g8:
                pop_bit(h8, board->bb[r]);
                pop_bit(h8, *pocc);
                set_bit(f8, board->bb[r]);
                set_bit(f8, *pocc);
                break;
        }
    }

    board->castling &= castlingUpdate[src];
    board->castling &= castlingUpdate[dest];
    
    board->isWhite = !board->isWhite;
    board->fullMove++;

    board->occ[both] = board->occ[black] | board->occ[white];
}

void san_move(Board* board, const char* san) {
    int move = from_san(san, board);
    make_move(board, move);
}

void copy_board(Board* new, const Board* old) {
    memcpy(new, old, sizeof(Board));
}

Board* create_board() {
    return malloc(sizeof(Board));
}
