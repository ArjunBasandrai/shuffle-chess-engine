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

#ifndef MOVES_H_
#define MOVES_H_
#include "helpers/moves_list.h"
#endif

#include "helpers/fen.h"
#include "helpers/movegen.h"

#ifndef U64
#define U64 unsigned long long
#endif

void init_all() {
    init_leaper_attacks();
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    // init_magic_numbers();
}

enum {
    all_moves, only_captures
};

static inline int make_move(int move, int move_flag) {

    // quiet move

    if (move_flag == all_moves) {

        copy_board();
        
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int double_push = get_move_double(move);
        int enpassant = get_move_enpassant(move);
        int castling = get_move_castling(move);

        pop_bit(bitboards[piece], source_square);
        set_bit(bitboards[piece], target_square);

        if (capture) {

            int start_piece, end_piece;
            if (side == white) {
                start_piece = p;
                end_piece = k;
            } else {
                start_piece = P;
                end_piece = K;
            }

            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
                if (get_bit(bitboards[bb_piece], target_square)) {
                    pop_bit(bitboards[bb_piece], target_square);
                    break;
                }
            }
        }

        if (promoted) {
            pop_bit(bitboards[(side == white) ? P : p], target_square);
            set_bit(bitboards[promoted], target_square);
        }

    }

    // capture move

    else {

        if (get_move_capture(move)) {
            make_move(move,all_moves);
        }

        else {
            return 0;
        }

    }
}

// Main driver
int main(){
    init_all();

    parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPpP/R3K2R b KQkq - 0 1 ");
    print_board();

    moves move_list[1];

    generate_moves(move_list);

    for (int move_count = 0; move_count<move_list->count; move_count++) {
        int move = move_list->moves[move_count];

        copy_board();
        make_move(move, all_moves);
        print_board();
        getchar();

        take_back();
    }

    return 0;
}