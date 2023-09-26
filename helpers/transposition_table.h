#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

#define hash_size 0x400000

// no hash entry
#define no_hash_entry 100000 // must be outside alpha beta window (-50000, 50000)

#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

typedef struct {
    U64 hash_key;
    int depth;
    int flag;
    int score;

} tt;

tt transposition_table[hash_size];

void clear_transposition_table() {
    for (int index = 0; index < hash_size; index++) {
        transposition_table[index].hash_key = 0;
        transposition_table[index].depth = 0;
        transposition_table[index].flag = 0;
        transposition_table[index].score = 0;
    }
}

static inline int read_hash_entry(int alpha, int beta, int depth) {
    tt *hash_entry = &transposition_table[hash_key % hash_size];
    if (hash_entry->hash_key == hash_key) {
        if (hash_entry->depth >= depth) {
            if (hash_entry->flag == hash_flag_exact) {
                return hash_entry->score;
            } else if ((hash_entry->flag == hash_flag_alpha) && (hash_entry->score <= alpha)) {
                return alpha;
            } else if ((hash_entry->flag == hash_flag_beta) && (hash_entry->score >= beta)) {
                return beta;
            } 
        }
    }
    return no_hash_entry;
}

static inline void write_hash_entry(int score, int depth, int hash_flag) {
    tt *hash_entry = &transposition_table[hash_key % hash_size];

    hash_entry->hash_key = hash_key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
}