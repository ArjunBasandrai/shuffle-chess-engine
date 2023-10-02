#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "bit_manipulation.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "board.h"
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
            
            switch (piece) {
                case P: score += pawn_score[square]; break;
                case N: score += knight_score[square]; break;
                case B: score += bishop_score[square]; break;
                case R: score += rook_score[square]; break;
                case K: score += king_score[square]; break;

                case p: score -= pawn_score[mirror_score[square]]; break;
                case n: score -= knight_score[mirror_score[square]]; break;
                case b: score -= bishop_score[mirror_score[square]]; break;
                case r: score -= rook_score[mirror_score[square]]; break;
                case k: score -= king_score[mirror_score[square]]; break;
            }

            pop_bit(bitboard, square);
        }
    }

    return (side == white) ? score : -score;
}