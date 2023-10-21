#include <stdio.h>
#include <string.h>

#include "movegen.h"

#include "board_constants.h"

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