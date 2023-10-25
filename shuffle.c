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
#include "src/books/book.h"
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
    init_book();
    close_book();
}

// Main driver
int main(){
    init_all();

    int debug = 0;

    if (debug) {
        parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        printf("Polykey: %d\n",polykey_from_board() == 0x463b96181691fc9c);

        parse_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
        printf("Polykey: %d\n",polykey_from_board() == 0x823c9b50fd114196);
        
        parse_fen("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
        printf("Polykey: %d\n",polykey_from_board() == 0x0756b94461c50fb0);

        parse_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
        printf("Polykey: %d\n",polykey_from_board() == 0x662fafb965db29d4);

        parse_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
        printf("Polykey: %d\n",polykey_from_board() == 0x22a48b5a8e47ff78);

        parse_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3");
        printf("Polykey: %d\n",polykey_from_board() == 0x652a607ca3f242c1);
        
        parse_fen("rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4");
        printf("Polykey: %d\n",polykey_from_board() == 0x00fdd303c946bdd9);

        parse_fen("rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3");
        printf("Polykey: %d\n",polykey_from_board() == 0x3c8123ea7b067637);
        
        parse_fen("rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4");
        printf("Polykey: %d\n",polykey_from_board() == 0x5c3f9b829b279560);
    } else {
        uci_loop();
        free(transposition_table);
    }

    return 0;
}