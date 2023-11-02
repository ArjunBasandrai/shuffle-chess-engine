#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "search.h"

#include "board_constants.h"
#include "io.h"
#include "zobrist.h"
#include "polyglot/polykeys.h"


int movetime = -1;
int m_time = -1;
int inc = 0;

void print_move_scores(moves *move_list, s_board *pos) {
    for (int count = 0; count < move_list->count; count++) {
        print_move(move_list->moves[count]);
        printf(" : %d\n", score_move(move_list->moves[count], pos));
    }
}

int search_position_thread(void *data) {
    s_search_input *search_data = (s_search_input*)data;
    s_board *pos = malloc(sizeof(s_board));
    memcpy(pos, search_data->pos, sizeof(s_board));
    search_position(search_data->depth, pos, search_data->info);
    free(pos);
    return 0;
}

void search_position(int depth, s_board *pos, s_info *info) {
    int score = 0;
    
    info->nodes = 0;

    info->stopped = 0;
    
    pos->follow_pv = 0;
    pos->score_pv = 0;
    
    memset(pos->killer_moves, 0, sizeof(pos->killer_moves));
    memset(pos->history_moves, 0, sizeof(pos->history_moves));
    memset(pos->pv_table, 0, sizeof(pos->pv_table));
    memset(pos->pv_length, 0, sizeof(pos->pv_length));
    
    int alpha = -infinity;
    int beta = infinity;

    pos->age++;

    int best_move = 0;
    if (engine_options->use_book == 1) {
        best_move = get_book_move(pos);
        if (best_move){
            printf("bestmove ");
            print_move(best_move);
            printf("\n");
            return;
        }
        engine_options->use_book == 0;
    }

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        if (info->stopped == 1) break;
        pos->follow_pv = 1;
        
        score = negamax(alpha, beta, current_depth, pos, info);

        if ((score <= alpha) || (score >= beta)) {
            alpha = -infinity;
            beta = infinity;
            continue;
        }

        alpha = score - 50;
        beta = score + 50;

        if (pos->pv_length[0]) {
            if (score > -mate_value && score < -mate_score) {
                printf("info score mate %d depth %d nodes %llu time %d pv ", -(score + mate_value)/2 - 1, current_depth, info->nodes, get_time_ms() - info->starttime);
            } else if (score > mate_score && score < mate_value) {
                printf("info score mate %d depth %d nodes %llu time %d pv ", (mate_value - score)/2 + 1, current_depth, info->nodes, get_time_ms() - info->starttime);
            } else {
                printf("info score cp %d depth %d nodes %llu time %d pv ", score, current_depth, info->nodes, get_time_ms() - info->starttime);
            }
            for (int count = 0; count < pos->pv_length[0]; count++)
            {
                print_move(pos->pv_table[0][count]);
                // printf(" ");
            }
            printf("\n");
        }
        
    }

    printf("bestmove ");
    print_move(pos->pv_table[0][0]);
    printf("\n");
}