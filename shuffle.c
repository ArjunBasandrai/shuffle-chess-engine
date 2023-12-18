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

void init_all(s_board *pos) {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
    init_random_keys();
    init_evaluation_masks();
    init_transposition_table(64, pos);
    init_poly_book();
}

// Main driver
int main(){
    s_board position[1];
    s_info info[1];
    position->enpassant = no_sq;
    position->age = 0;
    info->quit = 0;
    info->threads = 1;
    init_all(position);

    int debug = 1;

    if (debug) {
        parse_fen("8/R7/5k1P/5p2/p1p2P2/Pr1pPK1p/1P1P4/r7 b - -", position);
        print_board(position);
        // print_bitboard(get_attackers(position, g7));
        // evaluate(position);
        int move = encode_move(a4, b3, p, 0, 1, 0, 0, 0);
        printf("%c\n", ascii_pieces[get_captured_piece(move, position)]);
    } else {
        uci_loop(position, info);
        free(transposition_table);
        clean_poly_book();
    }

    return 0;
}