#include <chess.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const char* player_str[] = { "Black", "White" };

const char* sq_str[] = {
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8", "-"
};

const char* ascii_piece = "PNBRQKpnbrqk";
const char* unicode_piece[] = { "♙","♘","♗","♖","♕","♔","♟","♞","♝","♜","♛","♚" };

const int str_square(const char* str) {
    return (str[1] - '1') * 8 + str[0] - 'a';
}

const int chr_piece(const char chr) {
    switch (chr) {
        case 'P': return P;
        case 'N': return N;
        case 'B': return B;
        case 'R': return R;
        case 'Q': return Q;
        case 'K': return K;
        case 'p': return p;
        case 'n': return n;
        case 'b': return b;
        case 'r': return r;
        case 'q': return q;
        case 'k': return k;
        default:  return none;
    }
}

const void print_bb(const U64 bb) {
    for (int r=7; r >= 0; r--) {
        printf("%d: ", r+1);
        for (int c=0; c < 8; c++) {
            if ((bb & (1ULL << (r*8+c))) != 0) printf("1 ");
            else printf(". ");
        }
        if (r == 6) printf("\tdec: %llu", bb);
        if (r == 5) printf("\thex: 0x%llx", bb);
        printf("\n");
    }
    printf("   A B C D E F G H\n\n");
}

const int get_piece(const int sq, const Board* board) {
    for (int piece = P; piece <= k; piece++) {
        if (board->bb[piece] & 1ULL << sq) {
            return piece;
        }
    }
    return none;
}

const void print_board(const Board* board) {
    int piece;
    char fen[100];

    for (int rank = 7; rank >= 0; rank--) {
        printf("%d: ", rank+1);
        for (int file = 0; file < 8; file++) {
            piece = get_piece(rank * 8 + file, board);
            if (piece == none) printf(". ");
            else printf("%c ", ascii_piece[piece]);
        }
        switch (rank) {
            case 6:
                to_fen(fen, board);
                printf("\t%s", fen);
                break;
            case 4:
                printf("\t   To move: %s", player_str[board->isWhite]);
                break;
            case 3:
                printf("\t  Castling: ");
                if (board->castling == no_castle) printf("%c", '-');
                else {
                    if (board->castling & wk) printf("%c", 'K');
                    if (board->castling & wq) printf("%c", 'Q');
                    if (board->castling & bk) printf("%c", 'k');
                    if (board->castling & bq) printf("%c", 'q');
                }
                break;
            case 2:
                printf("\tEn passant: %s", sq_str[board->enPassant]);
                break;
            case 1:
                printf("\t Half Move: %d", board->halfMove);
                break;
            case 0:
                printf("\t Full Move: %d", board->fullMove);
                break;
        }
        printf("\n");
    }
    printf("   A B C D E F G H\n\n");

}

void to_fen(char* fen, const Board* board) {
    int empty;
    for (int rank = 7; rank >= 0; rank--) {
        empty = 0;
        for (int file = 0; file < 8; file++) {
            int piece = get_piece(rank * 8 + file, board);
            if (piece == none) empty++;
            else {
                if (empty != 0) *fen++ = '0' + empty;
                empty = 0;
                *fen++ = ascii_piece[piece];
            }
        }
        if (empty != 0) *fen++ = '0' + empty;
        *fen++ = '/';
    }
    *fen--;
    *fen++ = ' ';

    // player
    *fen++ = board->isWhite ? 'w' : 'b';
    *fen++ = ' ';

    // castling
    if (board->castling == no_castle) *fen++ = '-';
    else {
        if (board->castling & wk) *fen++ = 'K';
        if (board->castling & wq) *fen++ = 'Q';
        if (board->castling & bk) *fen++ = 'k';
        if (board->castling & bq) *fen++ = 'q';
    }
    *fen++ = ' ';
    
    // en passant
    if (board->enPassant == none) *fen++ = '-';
    else {
        *fen++ = sq_str[board->enPassant][0];
        *fen++ = sq_str[board->enPassant][1];
    }
    *fen++ = ' ';
    *fen = '\0';
    sprintf(fen, "%d %d", board->halfMove, board->fullMove);
}

void from_fen(const char* fen, Board* board) {

    for (int piece = P; piece <= k; piece++) board->bb[piece] = 0;
    
    int rank = 7, file = 0;
    for (; *fen != ' '; *fen++) {
        if (*fen == '/') {
            rank--, file = 0;
            continue;
        }
        
        int piece = chr_piece(*fen);
        if (piece == none) file += (*fen - '0');
        else {
            board->bb[piece] |= 1ULL << (rank * 8 + file);
            file++;
        }
    }
    *fen++;

    // player
    board->isWhite = *fen == 'w'; 
    fen += 2;

    // castling
    // board->castling = get_castle_rights(fen);
    board->castling = no_castle;

    if (*fen == '-') fen ++;
    else for (; *fen != ' '; *fen++) {
        if (*fen == 'K')      board->castling |= wk;
        else if (*fen == 'Q') board->castling |= wq;
        else if (*fen == 'k') board->castling |= bk;
        else if (*fen == 'q') board->castling |= bq;
    }
    *fen++;

    // en passant
    board->enPassant = none;

    if (*fen == '-') fen += 2;
    else {
        board->enPassant = str_square(fen);
        fen += 3;
    }

    // half move
    board->halfMove = atoi(fen);
    for (; *fen != ' '; *fen++);
    *fen++;

    // full move
    board->fullMove = atoi(fen);
    
    for (int player = black; player < both; player++) board->occ[player] = 0;
    for (int piece = P; piece <= K; piece++) board->occ[white] |= board->bb[piece];
    for (int piece = p; piece <= k; piece++) board->occ[black] |= board->bb[piece];

    board->occ[both] = board->occ[black] | board->occ[white];
}

Board* create_from_fen(const char* fen) {
    Board* board = (Board*)malloc(sizeof(Board));
    from_fen(fen, board);
    return board;
}
/*
void to_san(char* san, int move, const Board* board) {
    Moves* moves = &board->moves;
    int src = get_move_source(move);
    int dest = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted = get_move_promoted(move);
    bool sameRank = false, sameFile = false;
    if (get_move_castling(move)) {
        if (dest == g1 | dest == g8) printf("O-O\n");
        if (dest == c1 | dest == c8) printf("O-O-O\n");
        return;
    }
    for (int i = 0; i < moves->count; i++) {
        if (move != moves->moves[i] && 
            piece == get_move_piece(moves->moves[i]) && 
            dest == get_move_target(moves->moves[i])) {
            printf("rep: ");
            print_move(moves->moves[i]);
            int other = get_move_source(moves->moves[i]);
            if (src / 8 == other / 8) sameRank = true;
            if (src % 8 == other % 8) sameFile = true;
        }
    }

    if (piece != P && piece != p) printf("%c", ascii_piece[piece]);

    if (sameFile) printf("%c", 'a' + src % 8);
    if (sameRank) printf("%c", '1' + src / 8);

    if (get_move_capture(move)) printf("x");

    printf("%s", sq_str[dest]);

    if (promoted) printf("%c", ascii_piece[promoted]);

    printf("\n");
}
*/

#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double << 21) |    \
    (enpassant << 22) | \
    (castling << 23) \

int from_san(const char* san, const Board* board) {
    // check for castles
    // if first is a capital then
    int piece = *san;
    int src, dest;
    int rank = -1;
    int file = -1;
    
    if ('A' <= piece && piece <= 'Z') {
        piece = board->isWhite ? chr_piece(piece) : chr_piece(piece)+p;
        san++;
    } else piece = board->isWhite ? P : p;

    file = *san++ - 'a';
    rank = *san++ - '1';
    src = rank*8 + file;

    file = *san++ - 'a';
    rank = *san++ - '1';
    dest = rank*8 + file;
    
    // is !board->isWhite then make lower case
    // if no piece then pawn
    
    // if both rank and file known easy
    // if both rank and file unknown easy
    /*
    for (int i = 0; i < moves->count; i++) {
        if (piece == get_move_piece(moves->moves[i]) &&
            dest  == get_move_target(moves->moves[i])) {
            int other = get_move_target(moves->moves[i]);
            // assume true
            if (rank == -1 && file == -1) {
                dest = other;
                break;
            } else if (rank == -1 && other / 8 == rank)
            printf("rep: ");
            print_move(moves->moves[i]);
            int other = get_move_source(moves->moves[i]);
            // if (src / 8 == other / 8) sameRank = true;
            // if (src % 8 == other % 8) sameFile = true;
        }
    }
    */
    // look for x
    return encode_move(src, dest, piece, false, false, false, false, false);
}

