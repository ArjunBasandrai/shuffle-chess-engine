#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/bit_manipulation.h"
#include "src/board_constants.h"
#include "src/board.h"
#include "src/evaluation.h"
#include "src/fen.h"
#include "src/magic_num_generator.h"
#include "src/magic_numbers.h"
#include "src/masks.h"
#include "src/movegen.h"
#include "src/moves_list.h"
#include "src/perft.h"
#include "src/pre_calculated_tables.h"
#include "src/transposition_table.h"
#include "src/uci.h"
#include "src/zobrist.h"
#include "src/polyglot/polykeys.h"


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
    init_poly_book();
}

// Main driver
int main(){
    init_all();
    s_board position;
    position.enpassant = no_sq;
    int debug = 0;

    if (debug) {
        struct copy_pos ccc;
        struct copy_pos cc;
        parse_fen(start_position, &position);
        print_board(&position);
        copy_board(&position, &ccc);
        make_move(encode_move(e2,e3,P,0,0,0,0,0), all_moves, &position);
        copy_board(&position, &cc);
        make_move(encode_move(e7,e6,p,0,0,0,0,0), all_moves, &position);
        take_back(&position, &cc);
        make_move(encode_move(d2,d3,P,0,0,0,0,0), all_moves, &position);
        print_bitboard(ccc.bitboards_copy[P]);
        take_back(&position, &ccc);
        print_board(&position);
    } else {
        uci_loop(&position);
        free(transposition_table);
        clean_poly_book();
    }

    return 0;
}