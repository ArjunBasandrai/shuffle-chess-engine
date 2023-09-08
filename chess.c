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


void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
}

// Main driver
int main(){
    init_all();
    U64 occupancy = 0ULL;
    set_bit(occupancy,c5);
    set_bit(occupancy,f2);
    set_bit(occupancy,g7);
    set_bit(occupancy,b2);
    set_bit(occupancy,g5);
    set_bit(occupancy,e2);
    set_bit(occupancy,e7);

    print_bitboard(get_bishop_attacks(d4, occupancy));
    print_bitboard(get_rook_attacks(e5, occupancy));
    return 0;
}