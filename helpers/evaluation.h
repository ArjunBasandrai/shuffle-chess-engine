#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "helpers/bit_manipulation.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

static inline int evaluate() {
    int score = 0;

    U64 bitboard;
    int piece, square;

    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        bitboard = bitboards[bb_piece];

        while (bitboard) {
            piece = bb_piece;
            square = get_lsb_index(bitboard);

            score += material_score[piece];

            pop_bit(bitboard, square);
        }
    }

    return (side == white) ? score : -score;
}