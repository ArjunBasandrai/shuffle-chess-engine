#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "search.h"

#include "board_constants.h"
#include "zobrist.h"
#include "polyglot/polykeys.h"
#include "threading/tinycthread.h"

#define ASP_WIN_D 25

int movetime = -1;
int m_time = -1;
int inc = 0;

thrd_t worker_threads[max_threads];

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

void iterative_deepen(s_search_worker_data* worker_data) {
    
    int score = 0;
    int alpha = -infinity;
    int beta = infinity;
    int print = 0, delta;
    for (int current_depth = 1; current_depth <= worker_data->info->depth; current_depth++) {

        delta = ASP_WIN_D;

        if (current_depth > 4) {
            alpha = max(-infinity, score - delta);
            beta = min(infinity, score + delta);
        } 

        while (1) {
            worker_data->pos->follow_pv = 1;
            if (worker_data->info->stopped == 1) break;
            score = negamax(alpha, beta, current_depth, worker_data->pos, worker_data->info, 0);

            if (score > alpha && score < beta) break;

            if (score <= alpha) {
                beta = (alpha + beta) / 2;
                alpha = max(-infinity, alpha - delta);
            }

            else if (score >= beta) {
                beta = min(infinity, beta + delta);
            }

            delta += delta / 2;
        }

        if (worker_data->thread_id == 0 && worker_data->info->print_info == 1) {
            print = 1;
        }

        if (print) {
            if (worker_data->pos->pv_length[0]) {
                if (score > -mate_value && score < -mate_score) {
                    printf("info score mate %d depth %d nodes %llu time %d pv ", -(score + mate_value)/2 - 1, current_depth, worker_data->info->nodes, get_time_ms() - worker_data->info->starttime);
                } else if (score > mate_score && score < mate_value) {
                    printf("info score mate %d depth %d nodes %llu time %d pv ", (mate_value - score)/2 + 1, current_depth, worker_data->info->nodes, get_time_ms() - worker_data->info->starttime);
                } else {
                    printf("info score cp %d depth %d nodes %llu time %d pv ", score, current_depth, worker_data->info->nodes, get_time_ms() - worker_data->info->starttime);
                }
                for (int count = 0; count < worker_data->pos->pv_length[0]; count++)
                {
                    print_move(worker_data->pos->pv_table[0][count]);
                }
                printf("\n");
            }
        }
    }
}

int start_worker_thread(void *data) {
    s_search_worker_data *worker_data = (s_search_worker_data*)data;
    iterative_deepen(worker_data);
    if (worker_data->thread_id == 0 && worker_data->info->print_info == 1) {
        printf("bestmove ");
        print_move(worker_data->pos->pv_table[0][0]);
        printf("\n");
    }
    free(worker_data);
}

void setup_worker_data(int thread_id, thrd_t *worker_thread, s_board *pos, s_info *info, tt *hash_table) {
    s_search_worker_data *worker_data = malloc(sizeof(s_search_worker_data));
    worker_data->pos = malloc(sizeof(s_board));
    memcpy(worker_data->pos, pos, sizeof(s_board));
    worker_data->info = info;
    worker_data->hash_table = hash_table;
    worker_data->thread_id = thread_id;
    thrd_create(worker_thread, &start_worker_thread, (void*)worker_data);
}

void create_search_workers(s_board *pos, s_info *info, tt *hash_table) {
    for (int i = 0; i < info->threads; i++) {
        setup_worker_data(i, &worker_threads[i], pos, info, hash_table);
    }
}

void search_position(int depth, s_board *pos, s_info *info) {
   
    int best_move = 0;

    info->nodes = 0;
    info->stopped = 0;

    pos->follow_pv = 0;
    pos->score_pv = 0;
    pos->age++;
    
    memset(pos->killer_moves, 0, sizeof(pos->killer_moves));
    memset(pos->history_moves, 0, sizeof(pos->history_moves));
    memset(pos->pv_table, 0, sizeof(pos->pv_table));
    memset(pos->pv_length, 0, sizeof(pos->pv_length));
    
    int alpha = -infinity;
    int beta = infinity;

    if (engine_options->use_book == 1) {
        best_move = get_book_move(pos);
        if (best_move){
            printf("bestmove ");
            print_move(best_move);
            printf("\n");
            return;
        }
        engine_options->use_book = 0;
    }

    create_search_workers(pos, info , transposition_table);

    for (int i = 0; i <= info->threads; i++) {
        thrd_join(worker_threads[i], NULL);
    }
}