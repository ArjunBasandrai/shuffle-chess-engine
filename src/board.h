#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

#include "board_constants.h"

typedef struct {
     U64 bitboards[12];
    int side;
    int castle;
    int enpassant;
} s_board;

extern U64 occupancies[3];

void print_board(s_board *pos);
