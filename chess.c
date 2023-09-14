#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef STRING_H_
#define STRING_H_
#include <string.h>
#endif

#include "helpers/magic_num_generator.h"

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "helpers/bit_manipulation.h"
#endif

#ifndef MASKS_H_
#define MASKS_H_
#include "helpers/masks.h"
#endif

#ifndef MAGIC_NUMS_H_
#define MAGIC_NUMS_H_
#include "helpers/magic_numbers.h"
#endif

#ifndef PCTABLES_H_
#define PCTABLES_H_
#include "helpers/pre_calculated_tables.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "helpers/board.h"
#endif

#include "helpers/fen.h"
#include "helpers/movegen.h"
#include "helpers/moves.h"

#ifndef U64
#define U64 unsigned long long
#endif

void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
}

// Main driver
int main(){
    init_all();

    moves move_list[1];
    move_list->count = 0;

    add_move(move_list,encode_move(d7,d8,P,Q,0,0,0,0));
    add_move(move_list,encode_move(d7,d8,P,R,0,0,0,0));
    add_move(move_list,encode_move(d7,d8,P,B,0,0,0,0));
    add_move(move_list,encode_move(d7,d8,P,N,0,0,0,0));

    print_move_list(move_list);

    return 0;
}