#pragma once

#include <chess.h>
#include <stdbool.h>

extern const U64 Rank1;
extern const U64 Rank2;
extern const U64 Rank3;
extern const U64 Rank4;
extern const U64 Rank5;
extern const U64 Rank6;
extern const U64 Rank7;
extern const U64 Rank8;

extern const U64 FileA;
extern const U64 FileB;
extern const U64 FileC;
extern const U64 FileD;
extern const U64 FileE;
extern const U64 FileF;
extern const U64 FileG;
extern const U64 FileH;

#define set_bit(sq, bb) (bb |= 1ULL << sq)
#define get_bit(sq, bb) (bb & 1ULL << sq)
#define pop_bit(sq, bb) (bb &= ~(1ULL << sq))

typedef struct SMagic {
    U64 mask;
    U64 magic;
    int shift;
} SMagic;

typedef struct PawnInfo {
    U64 promoteRank;
    U64 secondRank;
    U64 attacks[64];
    U64 singleMask[64];
    int singleMove[64];
    U64 doubleMask[8];
    int doubleMove[8];
} PawnInfo;

extern const U64 rMagics[64];
extern const U64 bMagics[64];
extern PawnInfo pawnInfo[2];

extern SMagic bishopMagics[64];
extern SMagic rookMagics  [64];

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

void init_attacks();
