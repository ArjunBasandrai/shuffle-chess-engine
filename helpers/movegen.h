#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef PCTABLES_H_
#define PCTABLES_H_
#include "helpers/pre_calculated_tables.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "helpers/board.h"
#endif

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

static inline void generate_moves() {
    int source_square, target_square;

    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++) {
        bitboard = bitboards[piece];

        if (side == white) {
            if (piece == P) {
                while (bitboard)
                {
                    source_square = get_lsb_index(bitboard);
                    target_square = source_square - 8;

                    if (!(target_square < a8) && !(get_bit(occupancies[both],target_square))) {
                        if (source_square >= a7 && source_square <= h7) {
                            printf("white pawn promotion: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square]);
                        } else {
                            printf("white pawn push: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square]);

                            if ((source_square >= a2 && source_square <= h2) && !(get_bit(occupancies[both],target_square - 8))) {
                                printf("white double pawn push: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square - 8]);
                            }
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
            }
        } else {
            if (piece == p) {
                while (bitboard)
                {
                    source_square = get_lsb_index(bitboard);
                    target_square = source_square + 8;

                    if (!(target_square > h1) && !(get_bit(occupancies[both],target_square))) {
                        if (source_square >= a2 && source_square <= h2) {
                            printf("black pawn promotion: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square]);
                        } else {
                            printf("black pawn push: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square]);

                            if ((source_square >= a7 && source_square <= h7) && !(get_bit(occupancies[both],target_square + 8))) {
                                printf("black double pawn push: %s->%s\n",sqaure_to_coordinate[source_square],sqaure_to_coordinate[target_square + 8]);
                            }
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
                
            }
        }
    }
}