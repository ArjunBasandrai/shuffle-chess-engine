#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

#include "board_constants.h"

typedef struct {
    U64 bitboards[12];
    U64 occupancies[3];
    int side;
    int castle;
    int enpassant;

    int ply;
    int fifty;
    
    U64 hash_key;
} s_board;


void print_board(s_board *pos);
