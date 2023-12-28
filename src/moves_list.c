#include <stdio.h>
#include "bit_manipulation.h"
#include "board.h"
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

int get_captured_piece(int move, s_board *pos) {
    int target_square = get_move_target(move);
    for (int i = (pos->side^1)*6; i <= K + (pos->side^1)*6; i++) {
        if (get_bit(pos->bitboards[i], target_square)) {
            return i;
        }
    }
    return -1;
}

void print_move(int move) {
    if (promoted_pieces[get_move_promoted(move)]) {
        printf("%s%s%c ",square_to_coordinate[get_move_source(move)],
                        square_to_coordinate[get_move_target(move)],
                        promoted_pieces[get_move_promoted(move)]);
    } else {
        printf("%s%s ",square_to_coordinate[get_move_source(move)],
                square_to_coordinate[get_move_target(move)]);
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

        printf("    %s%s%c  %c      %d        %d       %d          %d\n",square_to_coordinate[get_move_source(move)],
                                                                        square_to_coordinate[get_move_target(move)],
                                                                        get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                        ascii_pieces[get_move_piece(move)],
                                                                        get_move_capture(move) ? 1 : 0,
                                                                        get_move_double(move) ? 1 : 0,
                                                                        get_move_enpassant(move) ? 1 : 0,
                                                                        get_move_castling(move) ? 1 : 0);
    }

    printf("\n    Total number of moves: %d\n",move_list->count);
}