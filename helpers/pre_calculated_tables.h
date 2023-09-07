#ifndef U64
#define U64 unsigned long long
#endif

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef MASKS_H_
#define MASKS_H_
#include "helpers/masks.h"
#endif

U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

void init_leaper_attacks() {
    for(int square = 0; square < 64; square++){
        pawn_attacks[white][square] = mask_pawn_attacks(square,white);
        pawn_attacks[black][square] = mask_pawn_attacks(square,black);

        knight_attacks[square] = mask_knight_attacks(square);

        king_attacks[square] = mask_king_attacks(square);
    }
}