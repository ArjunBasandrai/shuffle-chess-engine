#ifndef RANDOM_H_
#define RANDOM_H_
#include "random_unsigned.h"
#endif

#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "board.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

U64 piece_keys[12][64];
U64 enpassant_keys[64];
U64 castle_keys[16];
U64 side_key;

void init_random_keys() {
    random_state = 1804289383;
    for (int piece = P; piece <= k; piece++) {
        for (int square = 0; square < 64; square++) {
            piece_keys[piece][square] = get_random_U64_number();
        }
    }

    for (int square = 0; square < 64; square++) {
        enpassant_keys[square] = get_random_U64_number();
    }

    for (int index = 0; index < 16; index++) {
        castle_keys[index] = get_random_U64_number();
    }

    side_key = get_random_U64_number();
    
}

U64 generate_hash_keys() {
    U64 final_key = 0ULL;

    U64 bitboard;

    for (int piece = P; piece <= k; piece++) {
        bitboard = bitboards[piece];
        while (bitboard) {
            int square = get_lsb_index(bitboard);

            final_key ^= piece_keys[piece][square];

            pop_bit(bitboard, square);
        }
    }

    if (enpassant != no_sq) {
        final_key ^= enpassant_keys[enpassant];
    }

    final_key ^= castle_keys[castle];

    if (side == black) final_key ^= side_key;

    return final_key;
}