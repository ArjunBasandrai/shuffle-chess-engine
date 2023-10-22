#pragma once

#include "magic_numbers.h"
#include "board_constants.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];
extern U64 bishop_masks[64];
extern U64 rook_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 rook_attacks[64][4096];

void init_leaper_attacks();
void init_sliders_attacks(int bishop);

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

static inline U64 get_queen_attacks(int square, U64 occupancy) {
    U64 queen_attacks = 0ULL;
    U64 bishop_occupancies = occupancy;
    U64 rook_occupancies = occupancy;
    
    bishop_occupancies &= bishop_masks[square];
    bishop_occupancies *= bishop_magic_numbers[square];
    bishop_occupancies >>= (64 - bishop_relevant_bits[square]);

    queen_attacks |= bishop_attacks[square][bishop_occupancies];

    rook_occupancies &= rook_masks[square];
    rook_occupancies *= rook_magic_numbers[square];
    rook_occupancies >>= (64 - rook_relevant_bits[square]);

    queen_attacks |= rook_attacks[square][rook_occupancies];

    return queen_attacks;
}
