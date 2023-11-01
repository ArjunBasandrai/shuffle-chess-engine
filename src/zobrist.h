#pragma once

#include "board.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern U64 piece_keys[12][64];
extern U64 enpassant_keys[64];
extern U64 castle_keys[16];
extern U64 side_key;

void init_random_keys();

U64 generate_hash_keys(s_board *pos);