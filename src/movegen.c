#include <stdio.h>
#include <string.h>

#include "movegen.h"

#include "board_constants.h"

void copy_board(s_board *pos, struct copy_pos *cc) {
    memcpy(cc->bitboards_copy, pos->bitboards, sizeof(pos->bitboards)); 
    memcpy(cc->occupancies_copy, occupancies, sizeof(occupancies)); 
    cc->side_copy = pos->side, cc->enpassant_copy = pos->enpassant, cc->castle_copy = pos->castle; cc->fifty_copy=fifty;
    cc->hash_key_copy = hash_key;
}
void take_back(s_board *pos, struct copy_pos *cc) {
    memcpy(pos->bitboards, cc->bitboards_copy, sizeof(pos->bitboards)); 
    memcpy(occupancies,cc->occupancies_copy,sizeof(occupancies)); 
    pos->side = cc->side_copy, pos->enpassant = cc->enpassant_copy, pos->castle = cc->castle_copy; fifty = cc->fifty_copy;
    hash_key = cc->hash_key_copy;
}

void print_attacked_squares(int side, s_board *pos) {

    printf("\n");
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            if (!file) {
                printf("%d  ", 8 - rank);
            }
            printf("%d ",is_square_attacked(square, side,pos) ? 1 : 0);
        }
        printf("\n");
    }

    printf("\n   a b c d e f g h\n\n");

}