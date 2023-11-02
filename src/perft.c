#include <stdio.h>
#include <string.h>

#include "perft.h"

#include "board_constants.h"
#include "gettime.h"


void perft_test(int depth, s_board *pos, s_info *info) {

    printf("\n  Performance test\n\n");

    moves move_list[1];
    generate_moves(move_list, pos);

    int start = get_time_ms();

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        struct copy_pos ptcopy;
        copy_board(pos, &ptcopy);

        if (!make_move(move_list->moves[move_count], all_moves, pos)) { 
            continue;
        }

        long cumulative_nodes = info->nodes;

        perft_driver(depth-1, pos, info);

        long old_nodes = info->nodes - cumulative_nodes;

        take_back(pos, &ptcopy);

        // print the test results
        printf("  moves: ");
        print_move(move_list->moves[move_count]);
        printf(" nodes: %lld\n",old_nodes);
    } 

    printf("\n  Depth: %d\n", depth);
    printf("  Nodes: %lld\n", info->nodes);
    printf("  Time: %d ms\n", get_time_ms() - start);
}