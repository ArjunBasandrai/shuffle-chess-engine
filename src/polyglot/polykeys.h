#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

#define U64_poly(u) (u##ULL)

extern const U64 polyglot_keys[781];
extern const int polyglot_pieces[12];

extern U64 polykey_from_board();