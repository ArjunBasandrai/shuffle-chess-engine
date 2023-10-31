#include <stdio.h>
#include <string.h>

#include "perft.h"

#include "board_constants.h"
#include "gettime.h"

U64 nodes;

void perft_test(int depth, s_board *pos) {

    printf("\n  Performance test\n\n");

    moves move_list[1];
    generate_moves(move_list, pos);

    int start = get_time_ms();

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        copy_board(pos);

        if (!make_move(move_list->moves[move_count], all_moves, pos)) { 
            continue;
        }

        long cumulative_nodes = nodes;

        perft_driver(depth-1, pos);

        long old_nodes = nodes - cumulative_nodes;

        take_back(pos);

        // print the test results
        printf("  moves: ");
        print_move(move_list->moves[move_count]);
        printf(" nodes: %lld\n",old_nodes);
    } 

    printf("\n  Depth: %d\n", depth);
    printf("  Nodes: %lld\n", nodes);
    printf("  Time: %d ms\n", get_time_ms() - start);
}