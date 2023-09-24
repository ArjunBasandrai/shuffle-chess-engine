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

#define max_ply 64

int checkmate_score = -49000;
int stalemate_score = 0;

int ply;

int killer_moves[2][max_ply];
int history_moves[12][max_ply];

int pv_length[max_ply];
int pv_table[max_ply][max_ply];

int follow_pv, score_pv;

static inline void enable_pv_scoring(moves *move_list) {
    follow_pv = 0;
    for (int count = 0; count < move_list->count; count++) {
        if (pv_table[0][ply] == move_list->moves[count]) {
            score_pv = 1;
            follow_pv = 1;
        }
    }
}

static inline int score_move(int move) {

    if (score_pv) {
        if (pv_table[0][ply] == move) {
            score_pv = 0;
            
            return 20000;
        }
    }

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

    int found_pv = 0;
    pv_length[ply] = ply;

    if (depth == 0) {
        return quiescence(alpha, beta);
    }

    if (ply > max_ply) return evaluate();

    nodes++;

    int in_check = is_square_attacked((side == white) ? get_lsb_index(bitboards[K]) : get_lsb_index(bitboards[k]),side ^ 1);

    if (in_check) depth++;

    int legal_moves=0;
    int old_alpha = alpha;

    moves move_list[1];
    generate_moves(move_list);

    if (follow_pv) enable_pv_scoring(move_list);

    sort_moves(move_list);

    for (int count = 0; count < move_list->count; count++) {
        copy_board();
        ply++;

        if (make_move(move_list->moves[count], all_moves) == 0) {
            ply--;
            continue;
        }

        legal_moves++;

        int score;

        if (found_pv) {
            /* Once you've found a move with a score that is between alpha and beta,
               the rest of the moves are searched with the goal of proving that they are all bad.
               It's possible to do this a bit faster than a search that worries that one
               of the remaining moves might be good. */
            score = -negamax(-alpha-1,-alpha,depth - 1);
            /* If the algorithm finds out that it was wrong, and that one of the
               subsequent moves was better than the first PV move, it has to search again,
               in the normal alpha-beta manner.  This happens sometimes, and it's a waste of time,
               but generally not often enough to counteract the savings gained from doing the
               "bad move proof" search referred to earlier. */
            if ((score > alpha) && (score < beta)) {
                score = -negamax(-beta, -alpha, depth-1);
            }
        } else {
            score = -negamax(-beta, -alpha, depth-1);
        }

        ply--;
        take_back();

        // fail-hard beta cutoff
        if (score >= beta) {

            // processing killer moves

            if (get_move_capture(move_list->moves[count]) == 0) {
                killer_moves[1][ply] = killer_moves[0][ply];
                killer_moves[0][ply] = move_list->moves[count];
            }

            // node fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {

            // store history moves
            if (get_move_capture(move_list->moves[count]) == 0) {
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            }

            alpha = score;

            found_pv = 1;

            pv_table[ply][ply] = move_list->moves[count];

            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++) {
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }

            pv_length[ply] = pv_length[ply + 1];
        }
    }

    if (legal_moves == 0) {
        if (in_check) {
            return checkmate_score + ply;
        } else {
            return stalemate_score;
        }
    }

    // node fails high
    return alpha;
}

void search_position(int depth)
{
    int score = 0;
    
    nodes = 0;
    
    follow_pv = 0;
    score_pv = 0;
    
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));
    
    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        follow_pv = 1;
        
        score = negamax(-50000, 50000, current_depth);
        
        printf("info score cp %d depth %d nodes %ld pv ", score, current_depth, nodes);
        
        for (int count = 0; count < pv_length[0]; count++)
        {
            print_move(pv_table[0][count]);
            printf(" ");
        }
        
        printf("\n");
    }

    printf("bestmove ");
    print_move(pv_table[0][0]);
    printf("\n");
}