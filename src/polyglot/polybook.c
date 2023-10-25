#include <math.h>
#include <stdio.h>

#include "../bit_manipulation.h"
#include "../board_constants.h"
#include "../board.h"
#include "polykeys.h"

const int polyglot_pieces[12] = {
    1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

int has_enpassant_capture() {
    int sq_with_pawn = 0;
    int target_piece = (side == white) ? P : p;
    if (enpassant != no_sq) {
        if (side == white) {
            sq_with_pawn = enpassant + 8;
        } else {
            sq_with_pawn = enpassant - 8;
        }

        U64 bitboard = bitboards[target_piece];

        if (get_bit(bitboard, sq_with_pawn + 1)) return 1;
        else if (get_bit(bitboard, sq_with_pawn - 1)) return 1;
        else return 0;
    }
}

U64 polykey_from_board() {
    int sq = 0, rank = 0, file = 0;
    U64 final_key = 0ULL;
    U64 bitboard;
    int piece;
    int poly_piece = 0;
    int offset = 0;

    // piece keys
    for (int piece = P; piece <= k; piece++) {
        bitboard = bitboards[piece];
        while (bitboard) {
            int square = get_lsb_index(bitboard);
            rank = 7 - floor(square/8);
            file = square % 8;
            poly_piece = polyglot_pieces[piece];

            final_key ^= polyglot_keys[(64 * poly_piece) + (8 * rank) + file];

            pop_bit(bitboard, square);
        }
    }

    // castling keys
    offset = 768;
    if (castle & wk) final_key ^= polyglot_keys[offset + 0];
    if (castle & wq) final_key ^= polyglot_keys[offset + 1];
    if (castle & bk) final_key ^= polyglot_keys[offset + 2];
    if (castle & bq) final_key ^= polyglot_keys[offset + 3];

    // enpassant keys
    offset = 772;
    if (has_enpassant_capture()) {
        file = enpassant % 8;
        final_key ^= polyglot_keys[offset + file];
    }

    // side keys
    if (side == white) final_key ^= polyglot_keys[780];

    return final_key;
}

