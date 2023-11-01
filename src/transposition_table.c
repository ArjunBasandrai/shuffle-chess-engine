#include <stdlib.h>

#include "transposition_table.h"

#ifndef U64
#define U64 unsigned long long
#endif

int hash_entries = 0;
tt *transposition_table = NULL;

void data_check(const int move) {
    int depth = rand() % 64;
    int flag = rand() % 3;
    int score = rand() % 50000+20000;
    U64 data = fold_data(score, depth, flag, move);
    print_move(move);
    printf("\nmove: %d flag: %d score: %d depth: %d\n", move, flag, score, depth);
    printf("move: %d flag: %d score: %d depth: %d\n\n", get_hash_move(data), get_hash_flag(data), get_hash_score(data), get_hash_depth(data));
}

void temp_hash(char *fen) {
    s_board bb[1];
    parse_fen(fen, bb);

    moves move_list[1];
    generate_moves(move_list, bb);

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        struct copy_pos pdcopy;
        copy_board(bb, &pdcopy);

        if (!make_move(move_list->moves[move_count], all_moves, bb)) { 
            continue;
        }
        take_back(bb,&pdcopy);
        data_check(move_list->moves[move_count]);
    }
}

void verify_smp_entry(int score, int depth, int flag, int move, U64 hash_key, U64 smp_data, U64 smp_key) {
    U64 data = fold_data(score, depth, flag, move);
    U64 key = hash_key ^ data;

    if (data != smp_data) {printf("Data Error!\n"); exit(0);}
    if (key != smp_key) {printf("Key Error!\n"); exit(0);}

    int move2 = get_hash_move(data);
    int flag2 = get_hash_flag(data);
    int depth2 = get_hash_depth(data);
    int score2 = get_hash_score(data);

    if (move != move2) {printf("Move Error!\n"); exit(0);}
    if (flag != flag2) {printf("Flag Error!\n"); exit(0);}
    if (depth != depth2) {printf("Depth Error!\n"); exit(0);}
    if (score != score2) {printf("Score Error!\n"); exit(0);}
    
}

void clear_transposition_table(s_board *pos) {
    tt *hash_entry;
    for (hash_entry = transposition_table; hash_entry < transposition_table + hash_entries; hash_entry++) {
        hash_entry->hash_key = 0;
        hash_entry->depth = 0;
        hash_entry->flag = 0;
        hash_entry->score = 0;
        hash_entry->age = 0;
        hash_entry->smp_data = 0ULL;
        hash_entry->smp_key = 0ULL;
    }
    pos->age = 0;
}

void init_transposition_table(int mb, s_board *pos) {
    int hash_size = 0x100000 * mb;  
    hash_entries = hash_size / sizeof(tt);

    if (transposition_table != NULL) {
        // printf("Clearing tt memory...\n");
        free(transposition_table);
    }

    transposition_table = (tt*)malloc(hash_entries*sizeof(tt));

    if (transposition_table == NULL) {
        // printf("Couldn't allocate memory for tt, trying size %dMB\n"),mb/2;
        init_transposition_table(mb/2,pos);
    } else {
        clear_transposition_table(pos);
        // printf("tt is initialized with %d entries\n", hash_entries);
    }
}