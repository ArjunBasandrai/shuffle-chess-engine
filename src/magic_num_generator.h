#pragma once 

#ifndef U64
#define U64 unsigned long long
#endif

U64 generate_magic_number();

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);

// Magic numbers

U64 find_magic_number(int square, int relevant_bits, int bishop);

// void init_magic_numbers();
