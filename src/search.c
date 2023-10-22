#include <stdio.h>
#include <windows.h>

#include "search.h"

#include "board_constants.h"
#include "io.h"
#include "zobrist.h"

int killer_moves[2][max_ply];
int history_moves[12][max_ply];

int pv_length[max_ply];
int pv_table[max_ply][max_ply];

int follow_pv, score_pv;

int quit = 0;
int movestogo = 30;
int movetime = -1;
int time = -1;
int inc = 0;
int starttime = 0;
int stoptime = 0;
int timeset = 0;
int stopped = 0;

int input_waiting() {
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
        return dw;
    }
    
    else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
}


void read_input() {
    // bytes to read holder
    int bytes;
    
    // GUI/user input
    char input[256] = "", *endc;

    // "listen" to STDIN
    if (input_waiting()) {
        // tell engine to stop calculating
        stopped = 1;
        
        do
        {
            // read bytes from STDIN
            bytes=_read(fileno(stdin), input, 256);
        }
        
        // until bytes available
        while (bytes < 0);
        
        // searches for the first occurrence of '\n'
        endc = strchr(input,'\n');
        
        // if found new line set value at pointer to 0
        if (endc) *endc=0;
        
        // if input is available
        if (strlen(input) > 0) {
            // match UCI "quit" command
            if (!strncmp(input, "quit", 4)) {
                // tell engine to terminate exacution    
                quit = 1;
            }

            // // match UCI "stop" command
            else if (!strncmp(input, "stop", 4)) {
                // tell engine to terminate exacution
                quit = 1;
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

void search_position(int depth)
{
    int score = 0;
    
    nodes = 0;

    stopped = 0;
    
    follow_pv = 0;
    score_pv = 0;
    
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));
    
    int alpha = -infinity;
    int beta = infinity;

    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        if (stopped == 1) break;
        follow_pv = 1;
        
        score = negamax(alpha, beta, current_depth);

        if ((score <= alpha) || (score >= beta)) {
            alpha = -infinity;
            beta = infinity;
            continue;
        }

        alpha = score - 50;
        beta = score + 50;

        if (pv_length[0]) {
            if (score > -mate_value && score < -mate_score) {
                printf("info score mate %d depth %d nodes %llu time %d pv ", -(score + mate_value)/2 - 1, current_depth, nodes, get_time_ms() - starttime);
            } else if (score > mate_score && score < mate_value) {
                printf("info score mate %d depth %d nodes %llu time %d pv ", (mate_value - score)/2 + 1, current_depth, nodes, get_time_ms() - starttime);
            } else {
                printf("info score cp %d depth %d nodes %llu time %d pv ", score, current_depth, nodes, get_time_ms() - starttime);
            }
            for (int count = 0; count < pv_length[0]; count++)
            {
                print_move(pv_table[0][count]);
                // printf(" ");
            }
            printf("\n");
        }
        
    }

    printf("bestmove ");
    print_move(pv_table[0][0]);
    printf("\n");
}