#pragma once 

#ifndef U64
#define U64 unsigned long long
#endif

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))

// bit macros
#define set_bit(bitboard,square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard,square) (get_bit((bitboard),(square)) ? (bitboard) ^= (1ULL << (square)) : 0)

#define get_file(square) (square & 7)

#define abs(x) ((x) < 0 ? -(x) : (x))
#define file_distance(a, b) abs(get_file(a) - get_file(b))
#define rank_distance(a, b) abs((a >> 3) - (b >> 3))

#define distance(a, b) (max(file_distance(a, b), rank_distance(a, b)))
#define most_advanced(file, side) ((side) ? get_msb_index(file) : get_lsb_index(file))

#define color(square) ((square_color[square]) ? ~light_squares : light_squares)

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

static inline int get_msb_index(U64 bitboard) {
    if (bitboard) {
        int msb_index = 0;
        while (bitboard >>= 1) {
            msb_index++;
        }
        return msb_index;
    } else {
        return -1;
    }
}

void print_bitboard(U64 bitboard);