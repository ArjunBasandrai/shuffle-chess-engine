#pragma once

#include <stdio.h>

#include "board_constants.h"
#include "board.h"
#include "moves_list.h"
#include "movegen.h"
#include "fen.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern int hash_entries;

#define no_hash_entry 100000 // must be outside alpha beta window (-50000, 50000)

#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

#define get_hash_score(x) ((x & 0x1FFFF) - 50000 - 2000)
#define get_hash_depth(x) ((x >> 17) & 0x3F)
#define get_hash_flag(x) ((x >> 24) & 0x3)
#define get_hash_move(x) ((int)(x >> 26))
#define fold_data(score, depth, flag, move) ((score + 50000 + 2000) | (depth << 17) | (flag << 24) | ((U64)move << 26))

extern void data_check(const int move);
extern void temp_hash(char *fen);
extern void verify_smp_entry(int score, int depth, int flag, int move, U64 hash_key, U64 smp_data, U64 smp_key);

typedef struct {
    U64 smp_data;
    U64 smp_key;
    int age;
} tt;

extern tt *transposition_table;

void clear_transposition_table(s_board *pos);

void init_transposition_table(int mb, s_board *pos);

static inline int read_hash_entry(int alpha, int beta, int *best_move, int depth, s_board *pos) {
    tt *hash_entry = &transposition_table[pos->hash_key % hash_entries];
    U64 test_key = pos->hash_key ^ hash_entry->smp_data;
    if (hash_entry->smp_key == test_key) {
        
        int smp_depth = get_hash_depth(hash_entry->smp_data);
        int smp_flag = get_hash_flag(hash_entry->smp_data);
        int smp_move = get_hash_move(hash_entry->smp_data);
        int smp_score = get_hash_score(hash_entry->smp_data);

        if (smp_depth >= depth) {

            int score = smp_score;
            // retrieve score independent from the actual path from root to current position
            if (score < -mate_score) score += pos->ply;
            if (score > mate_score) score -= pos->ply;

            if (smp_flag == hash_flag_exact) {
                return score;
            } else if ((smp_flag == hash_flag_alpha) && (score <= alpha)) {
                return alpha;
            } else if ((smp_flag == hash_flag_beta) && (score >= beta)) {
                return beta;
            } 
            *best_move = smp_move;
        }
    }
    return no_hash_entry;
}

static inline void write_hash_entry(int score, int best_move, int depth, int hash_flag, s_board *pos) {
    tt *hash_entry = &transposition_table[pos->hash_key % hash_entries];

    int replace = 0;

    if (hash_entry->smp_key != 0ULL) {
        replace = 1;
    } else {
        if (hash_entry->age < pos->age) {
            replace = 1;
        } else if (get_hash_depth(hash_entry->smp_data) <= depth) {
            replace = 1;
        } 
    }

    if (!replace) return;

    // store score independent from the actual path from root to current position
    if (score < -mate_score) score -= pos->ply;
    if (score > mate_score) score += pos->ply;

    U64 smp_data = fold_data(score, depth, hash_flag, best_move);
    U64 smp_key = pos->hash_key ^ smp_data;

    hash_entry->age = pos->age;
    hash_entry->smp_data = smp_data;
    hash_entry->smp_key = smp_key;
}