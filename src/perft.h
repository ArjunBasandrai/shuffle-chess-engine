#pragma once

#include "moves_list.h"
#include "movegen.h"
#include "board.h"


#ifndef U64
#define U64 unsigned long long
#endif

extern U64 nodes;

static inline void perft_driver(int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }

    moves move_list[1];
    generate_moves(move_list);

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        copy_board();

        if (!make_move(move_list->moves[move_count], all_moves)) { 
            continue;
        }

        perft_driver(depth-1);

        take_back();

        // U64 hash_from_scratch = generate_hash_keys();

        // if (hash_key != hash_from_scratch) {
        //     printf("\nTake back\nmove: ");
        //     print_move(move_list->moves[move_count]);
        //     printf("\n");
        //     print_board();
        //     printf("hash key should be: %llx\n",hash_from_scratch);
        //     getchar();
        // }
    }
}

void perft_test(int depth);