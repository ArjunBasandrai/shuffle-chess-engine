#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef STRING_H_
#define STRING_H_
#include <string.h>
#endif

#include "src/magic_num_generator.h"

#ifndef CONST_H_
#define CONST_H_
#include "src/board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "src/bit_manipulation.h"
#endif

#ifndef MASKS_H_
#define MASKS_H_
#include "src/masks.h"
#endif

#ifndef MAGIC_NUMS_H_
#define MAGIC_NUMS_H_
#include "src/magic_numbers.h"
#endif

#ifndef PCTABLES_H_
#define PCTABLES_H_
#include "src/pre_calculated_tables.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "src/board.h"
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "src/moves_list.h"
#endif

#ifndef FEN_H_
#define FEN_H_
#include "src/fen.h"
#endif

#ifndef MOVEGEN_H_
#define MOVEGEN_H_
#include "src/movegen.h"
#endif

#ifndef PERFT_H_
#define PERFT_H_
#include "src/perft.h"
#endif

#include "src/uci.h"

#ifndef EVAL_H_
#define EVAL_H_
#include "src/evaluation.h"
#endif

#ifndef ZOBRIST_H_
#define ZOBRIST_H_
#include "src/zobrist.h"
#endif

#ifndef TRANSPOSE_H_
#define TRANSPOSE_H_
#include "src/transposition_table.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
    init_random_keys();
    init_evaluation_masks();
    init_transposition_table(64);
}

// Main driver
int main(){
    init_all();

    int debug = 0;

    if (debug) {
        parse_fen(start_position);
        print_board();
        printf("score: %d\n",evaluate());
    } else {
        uci_loop();
    }

    return 0;
}