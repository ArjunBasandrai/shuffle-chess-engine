#include <stdio.h>

#include "bit_manipulation.h"

void print_bitboard(U64 bitboard) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if (!file){
                printf(" %d ",8 - rank);
            }
            printf(" %d",get_bit(bitboard,square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n\n");
    printf("    Bitboard: %llud\n\n",bitboard);
}
