#pragma once

#include "../board.h"
#include "../search.h"

typedef struct {
    char *fen;
    float result;
} s_texel;

typedef struct {
    s_texel* positions;
    double* error;
    int start_idx;
    int end_idx;
    int batch_size;
    double K;
    int thread_id;
} s_texel_thread_data;

typedef struct {
    int* param;
    char* name;
} s_texel_params;

s_texel* read_files(char *fen_path, char* score_path, int* counter);
double get_error(s_texel* positions, int num_positions, double K);
double get_best_k(s_texel* positions, int counter);
void tune(s_texel* positions, int num_positions, s_board* position, s_info* info);