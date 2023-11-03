#pragma once

#include "moves_list.h"
#include "movegen.h"
#include "board.h"
#include "search.h"

#ifndef U64
#define U64 unsigned long long
#endif

static inline void perft_driver(int depth, s_board *pos, s_info *info) {
    if (depth == 0) {
        info->nodes++;
        return;
    }

    moves move_list[1];
    generate_moves(move_list, pos);

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        struct copy_pos pdcopy;
        copy_board(pos, &pdcopy);

        if (!make_move(move_list->moves[move_count], all_moves, pos)) { 
            continue;
        }

        perft_driver(depth-1, pos, info);

        take_back(pos,&pdcopy);

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

void perft_test(int depth, s_board *pos, s_info *info);