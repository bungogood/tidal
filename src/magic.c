#include <stdio.h>
#include <stdlib.h>
#include <chess.h>
#include <stdbool.h>

unsigned int seed = 1804289383;

unsigned int random_uint() {
    unsigned int number = seed;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number <<  5;
    seed = number;
    return number;
}

U64 random_U64() {
    U64 a,b,c,d;
    a = (U64)(random_uint()) & 0xFFFF;
    b = (U64)(random_uint()) & 0xFFFF;
    c = (U64)(random_uint()) & 0xFFFF;
    d = (U64)(random_uint()) & 0xFFFF;
    return a | (b << 16) | (c << 32) | (d << 48);
}

U64 random_U64_fewbits() {
    return random_U64() & random_U64() & random_U64();
}

typedef struct SMagic {
    U64 mask;
    U64 magic;
    int shift;
} SMagic;

SMagic bishopMagics[64];
SMagic rookMagics  [64];

U64 bishopAttacks[64] [512]; // 256 K
U64 rookAttacks  [64][4096]; // 2048K

int countBits(U64 b) {
    int r;
    for(r = 0; b; r++, b &= b - 1);
    return r;
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

U64 maskPerm(int index, int bits, U64 m) {
    int i, j;
    U64 result = 0ULL;
    for(i = 0; i < bits; i++) {
        j = pop_LSB(&m);
        if(index & (1 << i)) result |= (1ULL << j);
    }
    return result;
}

int magic_hash(U64 b, U64 magic, int bits) {
    return (int)((b * magic) >> (64 - bits));
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
        rookMagics[sq].shift = countBits(rookMagics[sq].mask);

        max_occ = (1 << rookMagics[sq].shift);

        for(int index = 0; index < max_occ; index++) {
            occ = maskPerm(index, rookMagics[sq].shift, rookMagics[sq].mask);
            rookAttacks[sq][index] = rookAttack(sq, occ);
        }
    }
}

void initBishopAttack() {
    int max_occ;
    U64 occ;

    for (int sq = 0; sq < 64; sq++) {
        bishopMagics[sq].mask  = bishopMask(sq);
        bishopMagics[sq].shift = countBits(bishopMagics[sq].mask);

        max_occ = (1 << bishopMagics[sq].shift);

        for(int index = 0; index < max_occ; index++) {
            occ = maskPerm(index, bishopMagics[sq].shift, bishopMagics[sq].mask);
            bishopAttacks[sq][index] = bishopAttack(sq, occ);
        }
    }
}

U64 find_magic(int sq, bool isBishop) {

    SMagic tmp = isBishop ? bishopMagics[sq] : rookMagics[sq];

    U64 occ[4096];
    U64* attacks = isBishop ? bishopAttacks[sq] : rookAttacks[sq];
    U64 used[4096];

    U64 mask = tmp.mask;
    int numBits = countBits(mask);

    int max_occ = (1 << tmp.shift);

    for(int i = 0; i < max_occ; i++) {
        occ[i] = maskPerm(i, tmp.shift, mask);
    }

    for(int count = 0; count < 100000000; count++) {

        tmp.magic = random_U64_fewbits();

        if(countBits((mask * tmp.magic) & 0xFF00000000000000ULL) < 6) continue;

        for(int i = 0; i < 4096; i++) used[i] = 0ULL;

        bool fail = false;

        for(int i = 0; !fail && i < max_occ; i++) {
            int magic_i = magic_hash(occ[i], tmp.magic, tmp.shift);
            
            if(used[magic_i] == 0ULL) {
                used[magic_i] = attacks[i];
            } else if(used[magic_i] != attacks[i]) {
                fail = true;
            }
        }

        if(!fail) return tmp.magic;
    }
    printf("***Failed***\n");
    return 0ULL;
}

enum { rook , bishop };

int main() {
    initRookAttack();
    initBishopAttack();

    printf("const U64 rMagic[64] = {\n");
    for(int sq = 0; sq < 64; sq++) {
        printf("    0x%llxULL,\n", find_magic(sq, rook));
    }
    printf("};\n\n");

    printf("const U64 bMagic[64] = {\n");
    for(int sq = 0; sq < 64; sq++) {
        printf("    0x%llxULL,\n", find_magic(sq, bishop));
    }
    printf("};\n\n");

    return 0;
}