#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef BITS_H_
#define BITS_H_
#include "bit_manipulation.h"
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

int killer_moves[2][64];
int history_moves[12][64];

static inline int score_move(int move) {
    if (get_move_capture(move)){
        /* init target_piece to P because in case of enpassant, there will be no piece at target_square*/
        int target_piece = P;

        int start_piece, end_piece;
        if (side == white) { start_piece = p; end_piece = k;} 
        
        else { start_piece = P; end_piece = K; }

        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
            if (get_bit(bitboards[bb_piece], get_move_target(move))) {
                target_piece = bb_piece;
                break;
            }
        }

        return mvv_lva[get_move_piece(move)][target_piece] + 10000; // + 10000 is added so that captures rank higher than killer moves
    } 
    
    else {
        // score 1st killer move

        if (killer_moves[0][ply] == move) {
            return 9000;
        }

        // score 2nd killer move

        else if (killer_moves[1][ply] == move) {
            return 8000;
        }

        // score history move

        else {
            return history_moves[get_move_piece(move)][get_move_target(move)];
        }
    }

    return 0;
}

static inline int sort_moves(moves *move_list) {
    int move_scores[move_list->count];
    for (int count = 0; count < move_list->count; count++) {
        move_scores[count] = score_move(move_list->moves[count]);
    }

    for (int current_move = 0; current_move < move_list->count; current_move++) {
        for (int next_move = current_move + 1; next_move < move_list->count; next_move++) {
            if (move_scores[current_move] < move_scores[next_move]) {
                int temp_score = move_scores[current_move];
                move_scores[current_move] = move_scores[next_move];
                move_scores[next_move] = temp_score;

                int temp_move = move_list->moves[current_move];
                move_list->moves[current_move] = move_list->moves[next_move];
                move_list->moves[next_move] = temp_move;
            }
        }
    }
}

void print_move_scores(moves *move_list) {
    for (int count = 0; count < move_list->count; count++) {
        print_move(move_list->moves[count]);
        printf(" : %d\n", score_move(move_list->moves[count]));
    }
}

static inline int quiescence(int alpha, int beta) {

    nodes++;

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
    sort_moves(move_list);

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

    if (in_check) depth++;

    int legal_moves=0;
    int best;
    int old_alpha = alpha;

    moves move_list[1];
    generate_moves(move_list);
    sort_moves(move_list);

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
            // processing killer moves

            killer_moves[1][ply] = killer_moves[0][ply];
            killer_moves[0][ply] = move_list->moves[count];

            // node fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {

            // store history moves

            history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;

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