#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

int hash_entries = 0;

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

tt *transposition_table = NULL;

void clear_transposition_table() {
    tt *hash_entry;
    for (hash_entry = transposition_table; hash_entry < transposition_table + hash_entries; hash_entry++) {
        hash_entry->hash_key = 0;
        hash_entry->depth = 0;
        hash_entry->flag = 0;
        hash_entry->score = 0;
    }
}

void init_transposition_table(int mb) {
    int hash_size = 0x100000 * mb;  
    hash_entries = hash_size / sizeof(tt);

    if (transposition_table != NULL) {
        printf("Clearing tt memory...\n");
        free(transposition_table);
    }

    transposition_table = (tt*)malloc(hash_entries*sizeof(tt));

    if (transposition_table == NULL) {
        printf("Couldn't allocate memory for tt, trying size %dMB\n"),mb/2;
        init_transposition_table(mb/2);
    } else {
        clear_transposition_table();
        printf("tt is initialized with %d entries\n", hash_entries);
    }
}

static inline int read_hash_entry(int alpha, int beta, int depth) {
    tt *hash_entry = &transposition_table[hash_key % hash_entries];
    if (hash_entry->hash_key == hash_key) {
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
        }
    }
    return no_hash_entry;
}

static inline void write_hash_entry(int score, int depth, int hash_flag) {
    tt *hash_entry = &transposition_table[hash_key % hash_entries];

    // store score independent from the actual path from root to current position
    if (score < -mate_score) score -= ply;
    if (score > mate_score) score += ply;

    hash_entry->hash_key = hash_key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
}