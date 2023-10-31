#include <stdio.h>
#include <string.h>

#include "movegen.h"

#include "board_constants.h"

void copy_board(s_board *pos) {
    memcpy(bitboards_copy, bitboards, sizeof(bitboards)); 
    memcpy(occupancies_copy, occupancies, sizeof(occupancies)); 
    side_copy = pos->side, enpaassant_copy = enpassant, castle_copy = castle; fifty_copy=fifty;
    hash_key_copy = hash_key;
}
void take_back(s_board *pos) {
    memcpy(bitboards, bitboards_copy, sizeof(bitboards)); 
    memcpy(occupancies,occupancies_copy,sizeof(occupancies)); 
    pos->side = side_copy, enpassant = enpaassant_copy, castle = castle_copy; fifty = fifty_copy;
    hash_key = hash_key_copy;
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