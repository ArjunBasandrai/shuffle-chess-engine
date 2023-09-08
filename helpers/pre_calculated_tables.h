#ifndef U64
#define U64 unsigned long long
#endif

#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef MASKS_H_
#define MASKS_H_
#include "masks.h"
#endif

U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];
U64 bishop_masks[64];
U64 rook_masks[64];
U64 bishop_attacks[64][512];
U64 rook_attacks[64][4096];

void init_leaper_attacks() {
    for(int square = 0; square < 64; square++){
        pawn_attacks[white][square] = mask_pawn_attacks(square,white);
        pawn_attacks[black][square] = mask_pawn_attacks(square,black);

        knight_attacks[square] = mask_knight_attacks(square);

        king_attacks[square] = mask_king_attacks(square);
    }
}

void init_sliders_attacks(int bishop) {
    for (int square = 0; square < 64; square++) {
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];
        int relevant_bits_count = count_bits(attack_mask);
        int occupancy_indices = 1 << relevant_bits_count;

        for (int index = 0; index < occupancy_indices; index++) {
            if (bishop) {
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square,occupancy);
            } else {
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square,occupancy);
            }
        }
    }
}

static inline U64 get_bishop_attacks(int square, U64 occupancy) {
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= (64 - bishop_relevant_bits[square]);

    return bishop_attacks[square][occupancy];
}

static inline U64 get_rook_attacks(int square, U64 occupancy) {
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= (64 - rook_relevant_bits[square]);

    return rook_attacks[square][occupancy];
}
