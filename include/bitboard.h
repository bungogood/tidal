#pragma once

#include <chess.h>
#include <stdbool.h>

extern U64 rowMask;
extern U64 colMask;

typedef struct SMagic {
    U64 mask;
    U64 magic;
    int shift;
} SMagic;

extern const U64 rMagics[64];
extern const U64 bMagics[64];

extern SMagic bishopMagics[64];
extern SMagic rookMagics  [64];

extern U64 pawnAttacks  [2][64];
extern U64 kingAttacks  [64];
extern U64 knightAttacks[64];
extern U64 bishopAttacks[64] [512]; // 256 K
extern U64 rookAttacks  [64][4096]; // 2048K

int countBits(U64 bb);
int getLSB(U64 bb);
int pop_LSB(U64 *bb);

U64 maskPerm(int index, int bits, U64 m);

U64 rookMask(int sq);
U64 bishopMask(int sq);
U64 rookAttack(int sq, U64 block);
U64 bishopAttack(int sq, U64 block);

U64 get_rook_attack(int sq, U64 occ);
U64 get_bishop_attack(int sq, U64 occ);
U64 get_queen_attack(int sq, U64 occ);

void init_attacks();
