#pragma once

#include "../board.h"
#include "../search.h"

typedef struct {
    char *fen;
    float result;
} s_texel;

s_texel* read_files(char *fen_path, char* score_path);
double get_error(s_texel* positions, int num_positions, double K, s_board* position, s_info* info);
void tune(s_texel* positions, double K, int num_positions, s_board* position, s_info* info);