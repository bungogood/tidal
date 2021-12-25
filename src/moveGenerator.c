#include <moveGenerator.h>
#include <board.h>
#include <stdio.h>

static inline U64 get_rook_attack(int sq, U64 occ) {
    occ  &= rookMagics[sq].mask;
    occ  *= rookMagics[sq].magic;
    occ >>= 64 - rookMagics[sq].shift;
    return rookAttacks[sq][occ];
}

static inline U64 get_bishop_attack(int sq, U64 occ) {
    occ  &= bishopMagics[sq].mask;
    occ  *= bishopMagics[sq].magic;
    occ >>= 64 - bishopMagics[sq].shift;
    return bishopAttacks[sq][occ];
}

static inline U64 get_queen_attack(int sq, U64 occ) {
    U64 bIndex = occ, rIndex = occ;
    rIndex  &= rookMagics[sq].mask;
    rIndex  *= rookMagics[sq].magic;
    rIndex >>= 64 - rookMagics[sq].shift;
    bIndex  &= bishopMagics[sq].mask;
    bIndex  *= bishopMagics[sq].magic;
    bIndex >>= 64 - bishopMagics[sq].shift;
    return rookAttacks[sq][rIndex] | bishopAttacks[sq][bIndex];
}

static inline bool isSquareAttacked(int sq, Board* board, int isWhite) {
    if (isWhite && (pawnInfo[black].attacks[sq] & board->bb[P])) return 1;
    if (!isWhite && (pawnInfo[white].attacks[sq] & board->bb[p])) return 1;
    if (knightAttacks[sq] & (isWhite ? board->bb[N] : board->bb[n])) return 1;
    if (get_bishop_attack(sq, board->occ[both]) & (isWhite ? board->bb[B] : board->bb[b])) return 1;
    if (get_rook_attack(sq, board->occ[both]) & (isWhite ? board->bb[R] : board->bb[r])) return 1;
    if (get_queen_attack(sq, board->occ[both]) & (isWhite ? board->bb[Q] : board->bb[q])) return 1;
    if (kingAttacks[sq] & (isWhite ? board->bb[K] : board->bb[k])) return 1;
    return 0;
}

const void print_attacked(Board* board, int isWhite) {
    for (int r=7; r >= 0; r--) {
        printf("%d: ", r+1);
        for (int c=0; c < 8; c++) {
            if (isSquareAttacked(r*8+c, board, isWhite)) printf("1 ");
            else printf(". ");
        }
        printf("\n");
    }
    printf("   A B C D E F G H\n\n");
}

void print_move(int move) {
    if (get_move_castling(move)) {
        int target = get_move_target(move);
        if (target == g1 | target == g8) printf("O-O\n");
        if (target == c1 | target == c8) printf("O-O-O\n");
        return;
    }
    int piece = get_move_piece(move);
    if (piece != P && piece != p) printf("%c", ascii_piece[piece % p]);
    printf("%s", sq_str[get_move_source(move)]);
    if (get_move_capture(move)) printf("x");
    printf("%s", sq_str[get_move_target(move)]);
    int promoted = get_move_promoted(move);
    if (promoted) printf("%c", ascii_piece[promoted]);
    if (get_move_enpassant(move)) printf(" e.p");
    if (get_move_double(move)) printf(" double");
    printf("\n");
}

void print_san(int move, Board* board) {
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
            // printf("rep: ");
            // print_move(moves->moves[i]);
            int other = get_move_source(moves->moves[i]);
            if (src / 8 == other / 8) sameRank = true;
            if (src % 8 == other % 8) sameFile = true;
        }
    }

    if (piece != P && piece != p) printf("%c", ascii_piece[piece % p]);

    if (sameFile) printf("%c", 'a' + src % 8);
    if (sameRank) printf("%c", '1' + src / 8);

    if (get_move_capture(move)) printf("x");

    printf("%s", sq_str[dest]);

    if (promoted) printf("%c", ascii_piece[promoted % p]);

    printf("\n");
}

void print_moves(Board* board) {
    Moves* moves = &board->moves;
    char str[8];
    int captures  = 0;
    int castles   = 0;
    int doubles   = 0;
    int enPassant = 0;
    int promotion = 0;
    for (int i = 0; i < moves->count; i++) {
        if (get_move_capture(moves->moves[i])) captures++;
        if (get_move_castling(moves->moves[i])) castles++;
        if (get_move_double(moves->moves[i])) doubles++;
        if (get_move_enpassant(moves->moves[i])) enPassant++;
        if (get_move_promoted(moves->moves[i])) promotion++;
        to_lan(str, moves->moves[i]);
        printf("%s ", str);
    }
    printf("\n\n");
    printf("\t(%d captures)", captures);
    printf("\t(%d castles)", castles);
    printf("\t(%d doubles)", doubles);
    printf("\t(%d enPassants)", enPassant);
    printf("\t(%d promotions)", promotion);
    printf("\n\n\t%d moves\n\n", moves->count);
}

static inline void add_move(Moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

typedef int Piece;

static inline Piece get_captured(Board* board, int sq, int start, int end) {
    for (int piece = start; piece <= end; piece++) {
        if (get_bit(sq, board->bb[piece])) return piece;
    }
    return none;
}

void generate_moves(Board* board) {
    
    int start, end;
    if (board->isWhite) start = p, end = k;
    else start = P, end = K;

    Moves* moves = &board->moves;
    moves->count = 0;
    // define source & target squares
    int src, dest, captured;
    
    // define current piece's bitboard copy & it's attacks
    U64 bitboard, attacks;
    
    // loop over all the board->bb
    for (int piece = P; piece <= k; piece++) {
        // init piece bitboard copy
        bitboard = board->bb[piece];
        // generate white pawns & white king castling moves
        if (board->isWhite == white) {
            // pick up white pawn board->bb index
            if (piece == P) {
                // loop over white pawns within white pawn bitboard
                while (bitboard) {
                    src = pop_LSB(&bitboard);
                    dest = src + 8;
                    // generate quite pawn moves
                    
                    if (!get_bit(dest, board->occ[both])) {
                        // pawn promotion
                        if (src >= a7 && src <= h7) {
                            add_move(moves, create_promtion(src,dest,P,Q));
                            add_move(moves, create_promtion(src,dest,P,R));
                            add_move(moves, create_promtion(src,dest,P,B));
                            add_move(moves, create_promtion(src,dest,P,N));
                        } else {
                            add_move(moves, create_quite(src,dest,P));
                            if ((src >= a2 && src <= h2) && !get_bit(dest+8, board->occ[both])) {
                                add_move(moves, create_double(src,dest+8,P));
                            }
                        }
                    }
                    // init pawn attacks bitboard
                    attacks = pawnInfo[board->isWhite].attacks[src] & board->occ[black];
                    // generate pawn captures
                    
                            // printf("cap, 0x%llx\n",board->occ[black]);
                    while (attacks) {
                        
                        // init target square
                        dest = pop_LSB(&attacks);
                        captured = get_captured(board, dest, start, end);
                        if (src >= a7 && src <= h7) {
                            add_move(moves, create_promtion_capture(src,dest,P,Q,captured));
                            add_move(moves, create_promtion_capture(src,dest,P,R,captured));
                            add_move(moves, create_promtion_capture(src,dest,P,B,captured));
                            add_move(moves, create_promtion_capture(src,dest,P,N,captured));
                        } else {
                            add_move(moves, create_capture(src,dest,P,captured));
                        }
                    }
                    
                    // generate board->enPassant captures
                    if (board->enPassant != none) {
                        // lookup pawn attacks and bitwise AND with board->enPassant square (bit)
                        U64 enpassant_attacks = pawnInfo[board->isWhite].attacks[src] & (1ULL << board->enPassant);
                        // make sure board->enPassant capture available
                        if (enpassant_attacks) {
                            dest = pop_LSB(&enpassant_attacks);
                            add_move(moves, create_enPassant(src,dest,P,p));
                        }
                    }
                }
            }
            
            // castling moves
            if (piece == K) {
                // king board->isWhite castling is available
                if (board->castling & wk) {
                    // make sure square between king and king's rook are empty
                    if (!get_bit(f1, board->occ[both]) && !get_bit(g1, board->occ[both])) {
                        // make sure king and the f1 squares are not under attacks
                        if (!isSquareAttacked(e1, board, black) && !isSquareAttacked(f1, board, black)) {
                            add_move(moves, create_castle(e1,g1,piece));
                        }
                    }
                }
                // queen board->isWhite castling is available
                if (board->castling & wq) {
                    // make sure square between king and queen's rook are empty
                    if (!get_bit(c1, board->occ[both]) && !get_bit(d1, board->occ[both])&& !get_bit(b1, board->occ[both])) {
                        // make sure king and the d1 squares are not under attacks
                        if (!isSquareAttacked(e1, board, black) && !isSquareAttacked(d1, board, black)) {
                            add_move(moves, create_castle(e1,c1,piece));
                        }
                    }
                }
            }
        } else {
            // pick up black pawn board->bb index
            if (piece == p) {
                // loop over white pawns within white pawn bitboard
                while (bitboard) {
                    // init source square
                    src = pop_LSB(&bitboard);
                    
                    // init target square
                    dest = src - 8;
                    
                    // generate quite pawn moves
                    if (!get_bit(dest, board->occ[both])) {
                        // pawn promotion
                        if (src >= a2 && src <= h2) {
                            add_move(moves, create_promtion(src,dest,p,q));
                            add_move(moves, create_promtion(src,dest,p,r));
                            add_move(moves, create_promtion(src,dest,p,b));
                            add_move(moves, create_promtion(src,dest,p,n));
                        } else {
                            add_move(moves, create_quite(src,dest,p));
                            if ((src >= a7 && src <= h7) && !get_bit(dest-8, board->occ[both])) {
                                add_move(moves, create_double(src,dest-8,p));
                            }
                        }
                    }
                    
                    // init pawn attacks bitboard
                    attacks = pawnInfo[board->isWhite].attacks[src] & board->occ[white];
                    
                    // generate pawn captures
                    while (attacks) {
                        // init target square
                        dest = pop_LSB(&attacks);
                        captured = get_captured(board, dest, start, end);
                        // pawn promotion
                        if (src >= a2 && src <= h2) {
                            add_move(moves, create_promtion_capture(src,dest,p,q,captured));
                            add_move(moves, create_promtion_capture(src,dest,p,r,captured));
                            add_move(moves, create_promtion_capture(src,dest,p,b,captured));
                            add_move(moves, create_promtion_capture(src,dest,p,n,captured));
                        } else {
                            add_move(moves, create_capture(src,dest,p,captured));
                        }
                    }
                    
                    // generate board->enPassant captures
                    if (board->enPassant != none) {
                        // lookup pawn attacks and bitwise AND with board->enPassant square (bit)
                        U64 enpassant_attacks = pawnInfo[board->isWhite].attacks[src] & (1ULL << board->enPassant);
                        
                        // make sure board->enPassant capture available
                        if (enpassant_attacks) {
                            dest = pop_LSB(&enpassant_attacks);
                            add_move(moves, create_enPassant(src,dest,p,P));
                        }
                    }
                    // pop ls1b from piece bitboard copy
                }
            }
            
            // castling moves
            if (piece == k) {
                // king board->isWhite castling is available
                if (board->castling & bk) {
                    // make sure square between king and king's rook are empty
                    if (!get_bit(f8, board->occ[both])&& !get_bit(g8, board->occ[both])) {
                        // make sure king and the f8 squares are not under attacks
                        if (!isSquareAttacked(e8, board, white) && !isSquareAttacked(f8, board, white)) {
                            add_move(moves, create_castle(e8,g8,piece));
                        }
                    }
                }
                
                // queen board->isWhite castling is available
                if (board->castling & bq) {
                    // make sure square between king and queen's rook are empty
                    if (!get_bit(c8, board->occ[both]) && !get_bit(d8, board->occ[both])&& !get_bit(b8, board->occ[both])) {
                        // make sure king and the d8 squares are not under attacks
                        if (!isSquareAttacked(e8, board, white) && !isSquareAttacked(d8, board, white)) {
                            add_move(moves, create_castle(e8,c8,piece));
                        }
                    }
                }
            }
        }
        
        // genarate knight moves
        if ((board->isWhite == white) ? piece == N : piece == n) {
            // loop over source squares of piece bitboard copy
            while (bitboard) {
                // init source square
                src = pop_LSB(&bitboard);
                
                // init piece attacks in order to get set of target squares
                attacks = knightAttacks[src] & ((board->isWhite == white) ? ~board->occ[white] : ~board->occ[black]);
                
                // loop over target squares available from generated attacks
                while (attacks) {
                    // init target square
                    dest = pop_LSB(&attacks);    
                    
                    // quite move
                    if (!get_bit(dest, ((board->isWhite == white) ? board->occ[black] : board->occ[white]))){
                        add_move(moves, create_quite(src,dest,piece));
                    } else {
                        captured = get_captured(board, dest, start, end);
                        add_move(moves, create_capture(src,dest,piece,captured));
                    }
                }
            }
        }
        
        // generate bishop moves
        if ((board->isWhite == white) ? piece == B : piece == b) {
            // loop over source squares of piece bitboard copy
            while (bitboard) {
                // init source square
                src = pop_LSB(&bitboard);
                
                // init piece attacks in order to get set of target squares
                attacks = get_bishop_attack(src, board->occ[both]) & ((board->isWhite == white) ? ~board->occ[white] : ~board->occ[black]);
                
                // loop over target squares available from generated attacks
                while (attacks) {
                    // init target square
                    dest = pop_LSB(&attacks);    
                    
                    // quite move
                    if (!get_bit(dest, ((board->isWhite == white) ? board->occ[black] : board->occ[white]))) {
                        add_move(moves, create_quite(src,dest,piece));
                    } else {
                        captured = get_captured(board, dest, start, end);
                        add_move(moves, create_capture(src,dest,piece,captured));
                    }
                }
            }
        }
        
        // generate rook moves
        if ((board->isWhite == white) ? piece == R : piece == r) {
            // loop over source squares of piece bitboard copy
            while (bitboard) {
                // init source square
                src = pop_LSB(&bitboard);
                
                // init piece attacks in order to get set of target squares
                attacks = get_rook_attack(src, board->occ[both]) & ((board->isWhite == white) ? ~board->occ[white] : ~board->occ[black]);
                
                // loop over target squares available from generated attacks
                while (attacks) {
                    // init target square
                    dest = pop_LSB(&attacks);    
                    
                    // quite move
                    if (!get_bit(dest, ((board->isWhite == white) ? board->occ[black] : board->occ[white]))) {
                        add_move(moves, create_quite(src,dest,piece));
                    } else {
                        captured = get_captured(board, dest, start, end);
                        add_move(moves, create_capture(src,dest,piece,captured));
                    }
                }
            }
        }
        
        // generate queen moves
        if ((board->isWhite == white) ? piece == Q : piece == q) {
            // loop over source squares of piece bitboard copy
            while (bitboard) {
                // init source square
                src = pop_LSB(&bitboard);
                
                // init piece attacks in order to get set of target squares
                attacks = get_queen_attack(src, board->occ[both]) & ((board->isWhite == white) ? ~board->occ[white] : ~board->occ[black]);
                
                // loop over target squares available from generated attacks
                while (attacks) {
                    // init target square
                    dest = pop_LSB(&attacks);    
                    
                    // quite move
                    if (!get_bit(dest, ((board->isWhite == white) ? board->occ[black] : board->occ[white]))) {
                        add_move(moves, create_quite(src,dest,piece));
                    } else {
                        captured = get_captured(board, dest, start, end);
                        add_move(moves, create_capture(src,dest,piece,captured));
                    }
                }
            }
        }

        // generate king moves
        if ((board->isWhite == white) ? piece == K : piece == k) {
            // loop over source squares of piece bitboard copy
            while (bitboard) {
                // init source square
                src = pop_LSB(&bitboard);
                
                // init piece attacks in order to get set of target squares
                attacks = kingAttacks[src] & ((board->isWhite == white) ? ~board->occ[white] : ~board->occ[black]);
                
                // loop over target squares available from generated attacks
                while (attacks) {
                    // init target square
                    dest = pop_LSB(&attacks);    
                    
                    // quite move
                    if (!get_bit(dest, ((board->isWhite == white) ? board->occ[black] : board->occ[white]))) {
                        add_move(moves, create_quite(src,dest,piece));
                    } else {
                        captured = get_captured(board, dest, start, end);
                        add_move(moves, create_capture(src,dest,piece,captured));
                    }
                }
            }
        }
    }
}

Board copy;

void generate_legal(Board* board) {
    generate_moves(board);

    U64* king = board->isWhite ? &copy.bb[K] : &copy.bb[k];
    int oldCount = board->moves.count;
    Moves* moves = &board->moves;
    board->moves.count = 0;
    for (int i = 0; i < oldCount; i++) {
        copy_board(&copy, board);
        make_move(&copy, moves->moves[i]);
        if (!isSquareAttacked(getLSB(*king), &copy, !board->isWhite)) {
            add_move(moves, moves->moves[i]);
        }
    }
}

void generate_captures(Board* board) {
    generate_legal(board);

    int oldCount = board->moves.count;
    board->moves.count = 0;

    for (int i = 0; i < oldCount; i++) {
        if (get_move_capture(board->moves.moves[i])) {
            add_move(&board->moves, board->moves.moves[i]);
        }
    }
}