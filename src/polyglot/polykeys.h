#pragma once

#include "../board.h"

#ifndef U64
#define U64 unsigned long long
#endif

#define U64_poly(u) (u##ULL)

extern const U64 polyglot_keys[781];
extern const int polyglot_pieces[12];

extern void init_poly_book();
extern void clean_poly_book();
extern int get_book_move(s_board *pos);
