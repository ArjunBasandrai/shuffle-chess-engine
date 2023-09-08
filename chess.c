#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#include <string.h>
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

#ifndef U64
#define U64 unsigned long long
#endif

#include "helpers/board.h"

void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
}


// Main driver
int main(){
    init_all();

    set_bit(bitboards[P],a2);
    set_bit(bitboards[P],b2);
    set_bit(bitboards[P],c2);
    set_bit(bitboards[P],d2);
    set_bit(bitboards[P],e2);
    set_bit(bitboards[P],f2);
    set_bit(bitboards[P],g2);
    set_bit(bitboards[P],h2);

    set_bit(bitboards[N],b1);
    set_bit(bitboards[N],g1);

    set_bit(bitboards[B],c1);
    set_bit(bitboards[B],f1);
    
    set_bit(bitboards[R],a1);
    set_bit(bitboards[R],h1);

    set_bit(bitboards[Q],d1);
    set_bit(bitboards[K],e1);


    set_bit(bitboards[p],a7);
    set_bit(bitboards[p],b7);
    set_bit(bitboards[p],c7);
    set_bit(bitboards[p],d7);
    set_bit(bitboards[p],e7);
    set_bit(bitboards[p],f7);
    set_bit(bitboards[p],g7);
    set_bit(bitboards[p],h7);

    set_bit(bitboards[n],b8);
    set_bit(bitboards[n],g8);

    set_bit(bitboards[b],c8);
    set_bit(bitboards[b],f8);
    
    set_bit(bitboards[r],a8);
    set_bit(bitboards[r],h8);

    set_bit(bitboards[q],d8);
    set_bit(bitboards[k],e8);

    enpassant = e3;
    castle |= wk;
    castle |= wq;
    castle |= bk;
    castle |= bq;

    for (int piece = P; piece <= k; piece++){
        print_bitboard(bitboards[piece]);
    }

    print_board();

    return 0;
}