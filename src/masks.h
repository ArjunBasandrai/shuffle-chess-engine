#pragma once

#include "board.h"

#ifndef U64
#define U64 unsigned long long
#endif

U64 mask_pawn_attacks(int square, int side);
U64 mask_knight_attacks(int square);
U64 mask_king_attacks(int square);
U64 mask_bishop_attacks(int square);
U64 mask_rook_attacks(int square);

U64 bishop_attacks_on_the_fly(int square,U64 block);
U64 rook_attacks_on_the_fly(int square, U64 block);

U64 get_attackers(const s_board *pos, int square);

U64 set_file_rank_mask(int file_number, int rank_number);

void init_evaluation_masks();