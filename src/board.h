#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

#define max_ply 64

#include "board_constants.h"

typedef struct {
    U64 bitboards[12];
    U64 occupancies[3];
    int side;
    int castle;
    int enpassant;

    int ply;
    int fifty;

    U64 repetitions_table[1000];
    int repetition_index;

    int pv_length[max_ply];
    int pv_table[max_ply][max_ply];

    int killer_moves[2][max_ply];
    int history_moves[max_ply][max_ply];

    int follow_pv;
    int score_pv;

    int age;
    U64 hash_key;
} s_board;

void print_board(s_board *pos);
