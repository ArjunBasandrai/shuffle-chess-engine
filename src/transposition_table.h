#pragma once

#include "board_constants.h"
#include "board.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern int hash_entries;

#define no_hash_entry 100000 // must be outside alpha beta window (-50000, 50000)

#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

typedef struct {
    U64 hash_key;
    int depth;
    int flag;
    int score;
    int best_move;
} tt;

extern tt *transposition_table;

void clear_transposition_table();

void init_transposition_table(int mb);

static inline int read_hash_entry(int alpha, int beta, int *best_move, int depth, s_board *pos) {
    tt *hash_entry = &transposition_table[pos->hash_key % hash_entries];
    if (hash_entry->hash_key == pos->hash_key) {
        if (hash_entry->depth >= depth) {

            int score = hash_entry->score;
            // retrieve score independent from the actual path from root to current position
            if (score < -mate_score) score += ply;
            if (score > mate_score) score -= ply;

            if (hash_entry->flag == hash_flag_exact) {
                return score;
            } else if ((hash_entry->flag == hash_flag_alpha) && (score <= alpha)) {
                return alpha;
            } else if ((hash_entry->flag == hash_flag_beta) && (score >= beta)) {
                return beta;
            } 
            *best_move = hash_entry->best_move;
        }
    }
    return no_hash_entry;
}

static inline void write_hash_entry(int score, int best_move, int depth, int hash_flag, s_board *pos) {
    tt *hash_entry = &transposition_table[pos->hash_key % hash_entries];

    // store score independent from the actual path from root to current position
    if (score < -mate_score) score -= ply;
    if (score > mate_score) score += ply;

    hash_entry->hash_key = pos->hash_key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
    hash_entry->best_move = best_move;
}