#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "moves_list.h"
#endif

#ifndef MOVEGEN_H_
#define MOVEGEN_H_
#include "movegen.h"
#endif

#ifndef EVAL_H_
#define EVAL_H_
#include "evaluation.h"
#endif

#ifndef PERFT_H_
#define PERFT_H_
#include "perft.h"
#endif

int checkmate_score = -49000;
int stalemate_score = 0;

int ply, best_move;

static inline int quiescence(int alpha, int beta) {

    int evaluation = evaluate();

    // fail-hard beta cutoff
    if (evaluation >= beta) {
        // node fails high
        return beta;
    }

    // found a better move
    if (evaluation > alpha) {
        alpha = evaluation;
    }

    moves move_list[1];
    generate_moves(move_list);

    for (int count = 0; count < move_list->count; count++) {
        copy_board();
        ply++;

        if (make_move(move_list->moves[count], only_captures) == 0) {
            ply--;
            continue;
        }

        int score = -quiescence(-beta, -alpha);
        ply--;
        take_back();

        // fail-hard beta cutoff
        if (score >= beta) {
            // node fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

static inline int negamax(int alpha, int beta, int depth) {
    if (depth == 0) {
        return quiescence(alpha, beta);
    }

    nodes++;

    int in_check = is_square_attacked((side == white) ? get_lsb_index(bitboards[K]) : get_lsb_index(bitboards[k]),side ^ 1);
    int legal_moves=0;
    int best;
    int old_alpha = alpha;

    moves move_list[1];
    generate_moves(move_list);

    for (int count = 0; count < move_list->count; count++) {
        copy_board();
        ply++;

        if (make_move(move_list->moves[count], all_moves) == 0) {
            ply--;
            continue;
        }

        legal_moves++;

        int score = -negamax(-beta, -alpha, depth-1);
        ply--;
        take_back();

        // fail-hard beta cutoff
        if (score >= beta) {
            // node fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;
            if (ply == 0) {
                best = move_list->moves[count];
            }
        }
    }

    if (legal_moves == 0) {
        if (in_check) {
            return checkmate_score + ply;
        } else {
            return stalemate_score;
        }
    }

    if (old_alpha != alpha) {
        best_move = best;
    }

    // node fails high
    return alpha;
}

void search_position(int depth) {
    int score = negamax(-50000, 50000, depth);

    if (best_move) {
        printf("info score cp %d depth %d nodes %ld\n", score, depth, nodes);
        printf("bestmove ");
        print_move(best_move);
        printf("\n");
    }
}