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

long nodes;

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
    }
}