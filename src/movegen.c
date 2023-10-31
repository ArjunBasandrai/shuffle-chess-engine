#include <stdio.h>
#include <string.h>

#include "movegen.h"

#include "board_constants.h"

void copy_board(s_board *pos, struct copy_pos *cc) {
    memcpy(cc->bitboards_copy, bitboards, sizeof(bitboards)); 
    memcpy(cc->occupancies_copy, occupancies, sizeof(occupancies)); 
    cc->side_copy = pos->side, cc->enpassant_copy = enpassant, cc->castle_copy = castle; cc->fifty_copy=fifty;
    cc->hash_key_copy = hash_key;
}
void take_back(s_board *pos, struct copy_pos *cc) {
    memcpy(bitboards, cc->bitboards_copy, sizeof(bitboards)); 
    memcpy(occupancies,cc->occupancies_copy,sizeof(occupancies)); 
    pos->side = cc->side_copy, enpassant = cc->enpassant_copy, castle = cc->castle_copy; fifty = cc->fifty_copy;
    hash_key = cc->hash_key_copy;
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