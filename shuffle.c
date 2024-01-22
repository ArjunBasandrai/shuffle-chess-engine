#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/bit_manipulation.h"
#include "src/board_constants.h"
#include "src/board.h"
#include "src/bench.h"
#include "src/evaluation.h"
#include "src/fen.h"
#include "src/magic_num_generator.h"
#include "src/magic_numbers.h"
#include "src/masks.h"
#include "src/movegen.h"
#include "src/moves_list.h"
#include "src/perft.h"
#include "src/see.h"
#include "src/see_test.h"
#include "src/texel/texel.h"
#include "src/pre_calculated_tables.h"
#include "src/transposition_table.h"
#include "src/uci.h"
#include "src/zobrist.h"
#include "src/polyglot/polykeys.h"

#ifndef U64
#define U64 unsigned long long
#endif

void init_all(s_board *pos) {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
    init_random_keys();
    init_evaluation_masks();
    init_transposition_table(64, pos);
    init_poly_book();

    for (int i = 0; i < max_ply; i++) {
        pos->pv_length[i] = 0;
        pos->repetitions_table[i] = 0;
        for (int j = 0; j < max_ply; j++) {
            pos->pv_table[i][j] = 0;
        }
    }
}

// Main driver
int main(int argc, char *argv[]){
    s_board position[1];
    s_info info[1];
    position->enpassant = no_sq;
    position->age = 0;
    info->quit = 0;
    info->threads = 1;
    info->print_info = 1;
    init_all(position);

    if (argc > 1) {
        // if argument "bench" is passed
        if (strcmp(argv[1], "bench") == 0) {
            bench(position, info);
            free(transposition_table);
            clean_poly_book();
            return 0;
        }
    }

    int debug = 0;
    // 25703396 nodes 8567798 nps
    if (debug) {
        int counter = 0;
        s_texel* positions = read_files("src/texel/fen.txt", "src/texel/result.txt", &counter);
        printf("Read %d positions\n", counter);
        tune(positions, counter, position, info);
    } else {
        uci_loop(position, info);
        free(transposition_table);
        clean_poly_book();
    }

    return 0;
}