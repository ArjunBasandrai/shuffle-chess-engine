#include <stdio.h>
#include <string.h>

#include "perft.h"

#include "board_constants.h"
#include "time.h"

void perft_test(int depth) {

    printf("\n  Performance test\n\n");

    moves move_list[1];
    generate_moves(move_list);

    int start = get_time_ms();

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        copy_board();

        if (!make_move(move_list->moves[move_count], all_moves)) { 
            continue;
        }

        long cumulative_nodes = nodes;

        perft_driver(depth-1);

        long old_nodes = nodes - cumulative_nodes;

        take_back();

        // print the test results
        printf("  moves: ");
        print_move(move_list->moves[move_count]);
        printf(" nodes: %lld\n",old_nodes);
    } 

    printf("\n  Depth: %d\n", depth);
    printf("  Nodes: %lld\n", nodes);
    printf("  Time: %d ms\n", get_time_ms() - start);
}