#pragma once

#include "board_constants.h"
#include "bit_manipulation.h"
#include "board.h"
#include "moves_list.h"
#include "movegen.h"
#include "evaluation.h"
#include "transposition_table.h"
#include "perft.h"
#include "gettime.h"

#define max_ply 64

extern int killer_moves[2][max_ply];
extern int history_moves[12][max_ply];

extern int pv_length[max_ply];
extern int pv_table[max_ply][max_ply];

extern int follow_pv, score_pv;

extern int quit;
extern int movestogo;
extern int movetime;
extern int m_time;
extern int inc;
extern int starttime;
extern int stoptime;
extern int timeset;
extern int stopped;

typedef struct {
    int depth;
    s_board *pos;
} s_search_input;

static void communicate();

// a bridge function to interact between search and GUI input
static void communicate() {
	// if time is up break here
    if (timeset == 1 && get_time_ms() > stoptime) {
		// tell engine to stop calculating
		stopped = 1;
	}
}

static inline void enable_pv_scoring(moves *move_list, s_board *pos) {
    follow_pv = 0;
    for (int count = 0; count < move_list->count; count++) {
        if (pv_table[0][pos->ply] == move_list->moves[count]) {
            score_pv = 1;
            follow_pv = 1;
        }
    }
}

static inline int score_move(int move, s_board *pos) {

    if (score_pv) {
        if (pv_table[0][pos->ply] == move) {
            score_pv = 0;
            
            return 20000;
        }
    }

    if (get_move_capture(move)){
        /* init target_piece to P because in case of enpassant, there will be no piece at target_square*/
        int target_piece = P;

        int start_piece, end_piece;
        if (pos->side == white) { start_piece = p; end_piece = k;} 
        
        else { start_piece = P; end_piece = K; }

        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
            if (get_bit(pos->bitboards[bb_piece], get_move_target(move))) {
                target_piece = bb_piece;
                break;
            }
        }

        return mvv_lva[get_move_piece(move)][target_piece] + 10000; // + 10000 is added so that captures rank higher than killer moves
    } 
    
    else {
        // score 1st killer move

        if (killer_moves[0][pos->ply] == move) {
            return 9000;
        }

        // score 2nd killer move

        else if (killer_moves[1][pos->ply] == move) {
            return 8000;
        }

        // score history move

        else {
            return history_moves[get_move_piece(move)][get_move_target(move)];
        }
    }

    return 0;
}

static inline int sort_moves(moves *move_list, int best_move, s_board *pos) {
    int move_scores[move_list->count];
    for (int count = 0; count < move_list->count; count++) {
        if (best_move == move_list->moves[count]) {
            move_scores[count] = 30000;
        } else {
        move_scores[count] = score_move(move_list->moves[count], pos);
        }
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
void print_move_scores(moves *move_list, s_board *pos);

static inline int is_repetition(s_board *pos) {

    for (int index = 0; index < pos->repetition_index; index++) {
        if (pos->repetitions_table[index] == pos->hash_key) {
            return 1;
        }
    }

    return 0;
}

static inline int quiescence(int alpha, int beta, s_board *pos) {

    if((nodes & 2047) == 0)
        // "listen" to the GUI/user input
		communicate();

    nodes++;

    if (pos->ply > max_ply - 1) {
        return evaluate(pos);
    }

    int evaluation = evaluate(pos);

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
    generate_moves(move_list, pos);
    sort_moves(move_list, 0, pos);

    for (int count = 0; count < move_list->count; count++) {
        struct copy_pos qcopy;
        copy_board(pos, &qcopy);
        pos->ply++;

        pos->repetition_index++;
        pos->repetitions_table[pos->repetition_index] = pos->hash_key;

        if (make_move(move_list->moves[count], only_captures, pos) == 0) {
            pos->ply--;
            pos->repetition_index--;
            continue;
        }

        int score = -quiescence(-beta, -alpha, pos);
        pos->ply--;
        pos->repetition_index--;
        take_back(pos, &qcopy);

        if(stopped == 1) return 0;

        // found a better move
        if (score > alpha) {
            alpha = score;

            // fail-hard beta cutoff
            if (score >= beta) {
                // node fails high
                return beta;
            }
        }
    }

    return alpha;
}

static inline int negamax(int alpha, int beta, int depth, s_board *pos) {

    int score;

    int best_move = 0;
    
    int hash_flag = hash_flag_alpha;

    if (pos->ply && is_repetition(pos) || pos->fifty >= 100) {
        return draw_score;
    }

    int pv_node = (beta - alpha > 1);

    // read hash from transposition table if not root ply and not a pv node
    if (pos->ply && (score = read_hash_entry(alpha, beta, &best_move, depth, pos)) != no_hash_entry && !pv_node) {
        // if move has already been searched and henc
        return score;
    }

    if((nodes & 2047 ) == 0)
        // "listen" to the GUI/user input
		communicate();
    
    pv_length[pos->ply] = pos->ply;

    if (depth == 0) {
        return quiescence(alpha, beta, pos);
    }

    if (pos->ply > max_ply - 1) return evaluate(pos);

    nodes++;

    int in_check = is_square_attacked((pos->side == white) ? get_lsb_index(pos->bitboards[K]) : get_lsb_index(pos->bitboards[k]),pos->side ^ 1, pos);

    if (in_check) depth++;

    int legal_moves=0;

    // Null Move Pruning
    if (depth >= 3 && !in_check && pos->ply) {
        struct copy_pos nmp;
        copy_board(pos, &nmp);

        pos->ply++;

        pos->repetition_index++;
        pos->repetitions_table[pos->repetition_index] = pos->hash_key;

        if (pos->enpassant != no_sq) pos->hash_key ^= enpassant_keys[pos->enpassant];

        pos->enpassant = no_sq;
        pos->side ^= 1;

        pos->hash_key ^= side_key;

        score = -negamax(-beta, -beta + 1, depth - 1 - 2, pos);

        pos->ply--;
        pos->repetition_index--;

        take_back(pos,&nmp);

        if (score >= beta) {
            return beta;
        }
    }

    // Razoring
    if (!pv_node && !in_check && depth <= 3) {
        score = evaluate(pos) + 125;
        int new_score;

        if (score < beta) {
            if (depth == 1) {
                new_score = quiescence(alpha, beta, pos);

                return (new_score > score) ? new_score : score;
            }

            score += 175;

            if (score < beta && depth <= 2) {
                new_score = quiescence(alpha, beta, pos);

                if (new_score < beta) {
                    return (new_score > score) ? new_score : score;
                }
            }
        }
    }

    moves move_list[1];
    generate_moves(move_list, pos);

    if (follow_pv) enable_pv_scoring(move_list, pos);

    sort_moves(move_list, best_move, pos);

    int moves_searched = 0;

    for (int count = 0; count < move_list->count; count++) {
        struct copy_pos ncopy;
        copy_board(pos, &ncopy);
        pos->ply++;

        pos->repetition_index++;
        pos->repetitions_table[pos->repetition_index] = pos->hash_key;

        if (make_move(move_list->moves[count], all_moves, pos) == 0) {
            pos->ply--;
            pos->repetition_index--;
            continue;
        }

        legal_moves++;

        if (moves_searched == 0) {
            score = -negamax(-beta, -alpha, depth - 1, pos);
        }
        
        else {
            if (
                moves_searched >= full_depth_moves &&
                depth >= reduction_limit &&
                in_check == 0 && 
                get_move_capture(move_list->moves[count]) == 0 &&
                get_move_promoted(move_list->moves[count]) == 0
                ) {
                score = -negamax(-alpha - 1, -alpha, depth - 2, pos);
                }
            
            else {score = alpha + 1;}
            
            // if found a better move during LMR
            if (score > alpha) {
                /* Once you've found a move with a score that is between alpha and beta,
                the rest of the moves are searched with the goal of proving that they are all bad.
                It's possible to do this a bit faster than a search that worries that one
                of the remaining moves might be good. */
                score = -negamax(-alpha - 1, -alpha, depth-1, pos);
            
                /* If the algorithm finds out that it was wrong, and that one of the
                subsequent moves was better than the first PV move, it has to search again,
                in the normal alpha-beta manner.  This happens sometimes, and it's a waste of time,
                but generally not often enough to counteract the savings gained from doing the
                "bad move proof" search referred to earlier. */
                if((score > alpha) && (score < beta)) {
                    score = -negamax(-beta, -alpha, depth-1, pos);
                }
            }
        }
        
        pos->ply--;
        pos->repetition_index--;
        take_back(pos, &ncopy);

        if(stopped == 1) return 0;

        moves_searched++;

        // found a better move
        if (score > alpha) {

            hash_flag = hash_flag_exact;

            best_move = move_list->moves[count];

            // store history moves
            if (get_move_capture(move_list->moves[count]) == 0) {
                history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
            }

            alpha = score;

            pv_table[pos->ply][pos->ply] = move_list->moves[count];

            for (int next_ply = pos->ply + 1; next_ply < pv_length[pos->ply + 1]; next_ply++) {
                pv_table[pos->ply][next_ply] = pv_table[pos->ply + 1][next_ply];
            }

            pv_length[pos->ply] = pv_length[pos->ply + 1];

            // fail-hard beta cutoff
            if (score >= beta) {

                write_hash_entry(beta, best_move, depth, hash_flag_beta,pos);

                // processing killer moves

                if (get_move_capture(move_list->moves[count]) == 0) {
                    killer_moves[1][pos->ply] = killer_moves[0][pos->ply];
                    killer_moves[0][pos->ply] = move_list->moves[count];
                }

                // node fails high
                return beta;
            }
        }
    }

    if (legal_moves == 0) {
        if (in_check) {
            return -mate_value + pos->ply;
        } else {
            return draw_score;
        }
    }

    write_hash_entry(alpha, best_move, depth, hash_flag,pos);

    // node fails high
    return alpha;
}

extern int search_position_thread(void *data);
extern void search_position(int depth, s_board *pos);