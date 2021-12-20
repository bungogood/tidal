#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <chess.h>
#include <bitboard.h>

const U64 rMagics[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};

const U64 bMagics[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};

SMagic bishopMagics[64];
SMagic rookMagics  [64];
PawnInfo pawnInfo[2];

U64 kingAttacks  [64];
U64 knightAttacks[64];
U64 bishopAttacks[64] [512]; // 256 K
U64 rookAttacks  [64][4096]; // 2048K

const U64 Rank1 = 0x00000000000000ffULL;
const U64 Rank2 = 0x000000000000ff00ULL;
const U64 Rank3 = 0x0000000000ff0000ULL;
const U64 Rank4 = 0x00000000ff000000ULL;
const U64 Rank5 = 0x000000ff00000000ULL;
const U64 Rank6 = 0x0000ff0000000000ULL;
const U64 Rank7 = 0x00ff000000000000ULL;
const U64 Rank8 = 0xff00000000000000ULL;

const U64 FileA = 0x0101010101010101ULL;
const U64 FileB = 0x0202020202020202ULL;
const U64 FileC = 0x0404040404040404ULL;
const U64 FileD = 0x0808080808080808ULL;
const U64 FileE = 0x1010101010101010ULL;
const U64 FileF = 0x2020202020202020ULL;
const U64 FileG = 0x4040404040404040ULL;
const U64 FileH = 0x8080808080808080ULL;

int countBits(U64 bb) {
    int count;
    for (count=0; bb; count++) bb &= bb - 1;
    return count;
}

int getLSB(U64 bb) { // returns index of LSB
    if (bb == 0) return -1;
    return countBits((bb & -bb) - 1);
}

const int BitTable[64] = {
    63, 30,  3, 32, 25, 41, 22, 33,
    15, 50, 42, 13, 11, 53, 19, 34,
    61, 29,  2, 51, 21, 43, 45, 10,
    18, 47,  1, 54,  9, 57,  0, 35,
    62, 31, 40,  4, 49,  5, 52, 26,
    60,  6, 23, 44, 46, 27, 56, 16,
     7, 39, 48, 24, 59, 14, 12, 55,
    38, 28, 58, 20, 37, 17, 36,  8
};

int pop_LSB(U64 *bb) {
    U64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

static int magic_hash(U64 occ, U64 magic, int bits) {
    return (int)((occ * magic) >> (64 - bits));
}

U64 maskPerm(int index, int bits, U64 m) {
    int i, j;
    U64 result = 0ULL;
    for(i = 0; i < bits; i++) {
        j = pop_LSB(&m);
        if(index & (1 << i)) result |= (1ULL << j);
    }
    return result;
}

void initPawnInfo() {
    /*
    * | 7 |   | 9 |
    * |   | i |   |
    * |-9 |   |-7 |
    */
    pawnInfo[white].secondRank  = Rank2;
    pawnInfo[black].secondRank  = Rank7;

    pawnInfo[white].promoteRank = Rank7;
    pawnInfo[black].promoteRank = Rank2;

    for (int sq = 0; sq < 64; sq++) {
        U64 set = 1ULL << sq;

        pawnInfo[white].singleMove[sq] = sq + 8;
        pawnInfo[white].singleMask[sq] = set << 8;
        pawnInfo[black].singleMove[sq] = sq - 8;
        pawnInfo[black].singleMask[sq] = set >> 8;
        if ((set & FileH) == 0) {
            pawnInfo[black].attacks[sq] |= set >> 7;
            pawnInfo[white].attacks[sq] |= set << 9;
        }
        if ((set & FileA) == 0) {
            pawnInfo[black].attacks[sq] |= set >> 9;
            pawnInfo[white].attacks[sq] |= set << 7;
        }
    }
    for (int i = 0; i < 8; i++) {
        pawnInfo[white].doubleMove[i] = (i + 24);
        pawnInfo[white].doubleMask[i] = 1ULL << (i + 24);
        pawnInfo[black].doubleMove[i] = (i + 32);
        pawnInfo[black].doubleMask[i] = 1ULL << (i + 32);
    }
}

void initKingAttack() {
    /*
    * | 7 | 8 | 9 |
    * |-1 | i | 1 |
    * |-9 |-8 |-7 |
    */
    for (int sq = 0; sq < 64; sq++) {
        U64 set = 1ULL << sq;

        kingAttacks[sq] = set >> 8 | set << 8;

        if ((set & FileH) == 0) kingAttacks[sq] |= set << 9 | set << 1 | set >> 7;
        if ((set & FileA) == 0) kingAttacks[sq] |= set >> 9 | set >> 1 | set << 7;
    }
}

void initKnightAttack() {
    /*
    *    | 15|   | 17|
    *  6 |   |   |   | 10
    *    |   | i |   |
    * -10|   |   |   | -6
    *    |-17|   |-15|
    */
    U64 FileAB = FileA | FileB;
    U64 FileGH = FileG | FileH;

    for (int sq = 0; sq < 64; sq++) {
        U64 set = 1ULL << sq;

        knightAttacks[sq] = 0;

        if ((set & FileA) == 0)  knightAttacks[sq] |= set << 15 | set >> 17;
        if ((set & FileAB) == 0) knightAttacks[sq] |= set <<  6 | set >> 10;
        if ((set & FileH) == 0)  knightAttacks[sq] |= set << 17 | set >> 15;
        if ((set & FileGH) == 0) knightAttacks[sq] |= set << 10 | set >>  6;
    }
}

U64 rookMask(int sq) {
    U64 result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for(r = rk+1; r <= 6; r++) result |= (1ULL << (fl + r*8));
    for(r = rk-1; r >= 1; r--) result |= (1ULL << (fl + r*8));
    for(f = fl+1; f <= 6; f++) result |= (1ULL << (f + rk*8));
    for(f = fl-1; f >= 1; f--) result |= (1ULL << (f + rk*8));
    return result;
}

U64 bishopMask(int sq) {
    U64 result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for(r=rk+1, f=fl+1; r<=6 && f<=6; r++, f++) result |= (1ULL << (f + r*8));
    for(r=rk+1, f=fl-1; r<=6 && f>=1; r++, f--) result |= (1ULL << (f + r*8));
    for(r=rk-1, f=fl+1; r>=1 && f<=6; r--, f++) result |= (1ULL << (f + r*8));
    for(r=rk-1, f=fl-1; r>=1 && f>=1; r--, f--) result |= (1ULL << (f + r*8));
    return result;
}

U64 rookAttack(int sq, U64 block) {
    U64 result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for(r = rk+1; r <= 7; r++) {
        result |= (1ULL << (fl + r*8));
        if(block & (1ULL << (fl + r*8))) break;
    }
    for(r = rk-1; r >= 0; r--) {
        result |= (1ULL << (fl + r*8));
        if(block & (1ULL << (fl + r*8))) break;
    }
    for(f = fl+1; f <= 7; f++) {
        result |= (1ULL << (f + rk*8));
        if(block & (1ULL << (f + rk*8))) break;
    }
    for(f = fl-1; f >= 0; f--) {
        result |= (1ULL << (f + rk*8));
        if(block & (1ULL << (f + rk*8))) break;
    }
    return result;
}

U64 bishopAttack(int sq, U64 block) {
    U64 result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for(r = rk+1, f = fl+1; r <= 7 && f <= 7; r++, f++) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) break;
    }
    for(r = rk+1, f = fl-1; r <= 7 && f >= 0; r++, f--) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) break;
    }
    for(r = rk-1, f = fl+1; r >= 0 && f <= 7; r--, f++) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) break;
    }
    for(r = rk-1, f = fl-1; r >= 0 && f >= 0; r--, f--) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) break;
    }
    return result;
}

void initRookAttack() {
    int max_occ;
    U64 occ;

    for (int sq = 0; sq < 64; sq++) {
        rookMagics[sq].mask  = rookMask(sq);
        rookMagics[sq].magic = rMagics[sq];
        rookMagics[sq].shift = countBits(rookMagics[sq].mask);

        int max_occ = (1 << rookMagics[sq].shift);

        for(int index = 0; index < max_occ; index++) {
            U64 occ = maskPerm(index, rookMagics[sq].shift, rookMagics[sq].mask);
            int magic_index = magic_hash(occ, rookMagics[sq].magic, rookMagics[sq].shift);
            rookAttacks[sq][magic_index] = rookAttack(sq, occ);
        }
    }
}

void initBishopAttack() {
    for (int sq = 0; sq < 64; sq++) {
        bishopMagics[sq].mask  = bishopMask(sq);
        bishopMagics[sq].magic = bMagics[sq];
        bishopMagics[sq].shift = countBits(bishopMagics[sq].mask);

        int max_occ = (1 << bishopMagics[sq].shift);

        for(int index = 0; index < max_occ; index++) {
            U64 occ = maskPerm(index, bishopMagics[sq].shift, bishopMagics[sq].mask);
            int magic_index = magic_hash(occ, bishopMagics[sq].magic, bishopMagics[sq].shift);
            bishopAttacks[sq][magic_index] = bishopAttack(sq, occ);
        }
    }
}

void init_attacks() {
    initPawnInfo();
    initKingAttack();
    initKnightAttack();
    initRookAttack();
    initBishopAttack();
}
