#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#define encode_move(source,target,piece,promoted,capture,double,enpassant,castling) \
    (source) | (target << 6) | \
    (piece << 12) | (promoted << 16) | \
    (capture << 20) | (double << 21) | \
    (enpassant << 22) | (castling << 23)

typedef struct {
    int moves[256];
    int count;
} moves;

#define get_move_source(move) (move & 0x3f)
#define get_move_target(move) ((move & 0xfc0)) >> 6
#define get_move_piece(move) ((move & 0xf000)) >> 12
#define get_move_promoted(move) ((move & 0xf0000)) >> 16
#define get_move_capture(move) (move & 0x100000)
#define get_move_double(move) (move & 0x200000)
#define get_move_enpassant(move) (move & 0x400000)
#define get_move_castling(move) (move & 0x800000)


static inline void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

char promoted_pieces[] = {
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [q] = 'q',
    [r] = 'r',
    [b] = 'b',
    [n] = 'n'
};

// for UCI
void print_move(int move) {
    printf("%s%s%c",sqaure_to_coordinate[get_move_source(move)],
                    sqaure_to_coordinate[get_move_target(move)],
                    promoted_pieces[get_move_promoted(move)]);
}

void print_move_list(moves *move_list) {

    if (!move_list->count) {
        printf("\n    No moves in move list!\n\n");
        return;
    }

    printf("\n    move   piece  capture  double  enpassant  castling\n\n");
    for (int move_count = 0; move_count < move_list->count; move_count++) {
        int move = move_list->moves[move_count];

        printf("    %s%s%c  %c      %d        %d       %d          %d\n",sqaure_to_coordinate[get_move_source(move)],
                                                                        sqaure_to_coordinate[get_move_target(move)],
                                                                        get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                        ascii_pieces[get_move_piece(move)],
                                                                        get_move_capture(move) ? 1 : 0,
                                                                        get_move_double(move) ? 1 : 0,
                                                                        get_move_enpassant(move) ? 1 : 0,
                                                                        get_move_castling(move) ? 1 : 0);
    }

    printf("\n    Total number of moves: %d\n",move_list->count);
}