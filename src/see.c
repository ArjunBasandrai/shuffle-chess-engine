#include <stdio.h>
#include "see.h"
#include "bit_manipulation.h"
#include "board_constants.h"
#include "masks.h"
#include "movegen.h"
#include "moves_list.h"

U64 get_least_valuable_piece(U64 attadef, int side, int *piece, const s_board *pos) {
    for (int p_trav = P; p_trav <= K; p_trav++) {
        int offset = side * 6;
        U64 bb = pos->bitboards[p_trav + offset] & attadef;
        if (bb) {
            *piece = p_trav + offset;
            return bb & -bb;
        }
    }
    return 0ULL;
}

int see(const s_board *pos, const int move) {
    int gain[32], d = 0;
    U64 diagnols = pos->bitboards[B] | pos->bitboards[Q] | pos->bitboards[b] | pos->bitboards[q];
    U64 horizontals = pos->bitboards[R] | pos->bitboards[Q] | pos->bitboards[r] | pos->bitboards[q];
    U64 sliders = diagnols | horizontals;
    U64 fromSet = 1ULL << get_move_source(move);
    U64 occ = pos->occupancies[both];
    U64 attadef = get_attackers(pos, get_move_target(move));
    int piece = get_move_piece(move);
    gain[d] = (get_captured_piece(move, pos)) != -1 ? abs(material_score[opening][get_captured_piece(move, pos)]) : 0;
    int side = pos->side;
    do {
        d++;
        gain[d] = abs(material_score[opening][piece]) - gain[d - 1];
        // if (max(-gain[d - 1], gain[d]) < 0) break;
        occ ^= fromSet;
        attadef ^= fromSet;
        if (fromSet & sliders) {
            if (piece == R || piece == r || piece == Q || piece == q) {
                attadef |= rook_attacks_on_the_fly(get_move_target(move), occ) & horizontals;
            } 
            if (piece == B || piece == b || piece == Q || piece == q) {
                attadef |= bishop_attacks_on_the_fly(get_move_target(move), occ) & diagnols;
            }
            attadef ^= fromSet;
        }
        attadef &= occ;
        side ^= 1;
        fromSet = get_least_valuable_piece(attadef, side, &piece, pos);
    } while (fromSet);

    while (--d)
        gain[d-1]= -max(-gain[d-1], gain[d]);
   return gain[0];
}
