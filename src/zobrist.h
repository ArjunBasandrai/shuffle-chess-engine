#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

U64 piece_keys[12][64];
U64 enpassant_keys[64];
U64 castle_keys[16];
U64 side_key;

void init_random_keys();

U64 generate_hash_keys();