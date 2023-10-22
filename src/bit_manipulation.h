#pragma once 

#ifndef U64
#define U64 unsigned long long
#endif

// bit macros
#define set_bit(bitboard,square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard,square) (get_bit((bitboard),(square)) ? (bitboard) ^= (1ULL << (square)) : 0)

static inline int count_bits(U64 bitboard){
    int count = 0;

    while (bitboard) {
        count++;
        bitboard &= bitboard - 1;
    }

    return count;
}

static inline int get_lsb_index(U64 bitboard){
    if (bitboard){
        return count_bits((bitboard & -bitboard) - 1);
    } else {
        return -1;
    }
}

void print_bitboard(U64 bitboard);