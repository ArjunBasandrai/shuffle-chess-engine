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

#ifndef U64
#define U64 unsigned long long
#endif

void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
}

static inline int is_square_attacked(int square, int side) {

    if ((side == white) && (pawn_attacks[black][square] & bitboards[P])) return 1;

    if ((side == black) && (pawn_attacks[white][square] & bitboards[p])) return 1;

    if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) return 1;

    if (get_bishop_attacks(square,occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;

    if (get_rook_attacks(square,occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;

    if (get_queen_attacks(square,occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;


    if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) return 1;

    return 0;
}

void print_attacked_squares(int side) {
    printf("\n");
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            if (!file) {
                printf("%d  ", 8 - rank);
            }
            printf("%d ",is_square_attacked(square, side) ? 1 : 0);
        }
        printf("\n");
    }

    printf("\n   a b c d e f g h\n\n");

}

// Main driver
int main(){
    init_all();

    parse_fen(tricky_position);
    print_board();
    print_attacked_squares(white);

    return 0;
}