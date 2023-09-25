#ifndef STRING_H_
#define STRING_H_
#include <string.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "helpers/moves_list.h"
#endif

#ifndef MOVEGEN_H_
#define MOVEGEN_H_
#include "helpers/movegen.h"
#endif

#include "time.h"

long long nodes;

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