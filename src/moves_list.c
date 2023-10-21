#include <stdio.h>
#include "moves_list.h"

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

void print_move(int move) {
    if (promoted_pieces[get_move_promoted(move)]) {
        printf("%s%s%c ",sqaure_to_coordinate[get_move_source(move)],
                        sqaure_to_coordinate[get_move_target(move)],
                        promoted_pieces[get_move_promoted(move)]);
    } else {
        printf("%s%s ",sqaure_to_coordinate[get_move_source(move)],
                sqaure_to_coordinate[get_move_target(move)]);
    }
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