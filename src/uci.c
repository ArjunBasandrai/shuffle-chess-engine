#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uci.h"

#include "board_constants.h"
#include "moves_list.h"
#include "movegen.h"
#include "search.h"
#include "fen.h"
#include "transposition_table.h"
#include "gettime.h"
#include "polyglot/polykeys.h"

#include "threading/tinycthread.h"

thrd_t main_search_thread;

thrd_t launch_search_thread(int depth, s_board *pos, s_info *info) {
    s_search_input *search_data = malloc(sizeof(s_search_input));
    search_data->depth = depth;
    search_data->pos = pos;
    search_data->info = info;

    thrd_t th;
    thrd_create(&th, &search_position_thread, (void*)search_data);

    return th;
}

void join_search_thread(s_info *info) {
    info->stopped = 1;
    thrd_join(main_search_thread, NULL);
}

void reset_time_control(s_info *info) {
    info->quit = 0;
    info->movestogo = 30;
    movetime = -1;
    m_time = -1;
    inc = 0;
    info->starttime = 0;
    info->stoptime = 0;
    info->timeset = 0;
    info->stopped = 0;
}

int parse_move(char *move_string, s_board *pos) {
    moves move_list[1];
    generate_moves(move_list, pos);

    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

    for (int move_count = 0; move_count < move_list->count; move_count++) {
        int move = move_list->moves[move_count];

        if (source_square == get_move_source(move) && target_square == get_move_target(move)) {
            int promoted_piece = get_move_promoted(move);

            if (promoted_piece) {
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q') {
                    return move;
                }

                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r') {
                    return move;
                }

                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b') {
                    return move;
                }

                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n') {
                    return move;
                }

                continue;
            }
            
            return move;  
        }
    }

    return 0;

    // printf("source square: %s\n",square_to_coordinate[target_square]);
}


void parse_position(char *command, s_board *pos) {
    command += 9;
    char *current_char = command;

    // parse UCI startpos command
    if (strncmp(command, "startpos", 8) == 0) {
        parse_fen(start_position, pos);
    } 
    
    // parse UCI fen command
    else {
        current_char = strstr(command, "fen");

        if (current_char == NULL) {
            parse_fen(start_position, pos);
        }

        else {
            current_char += 4;

            parse_fen(current_char, pos);
        }
    }

    current_char = strstr(command,"moves");

    if (current_char != NULL) {

        current_char += 6;
        int make_line = 1;
        while(*current_char) {
            int move = parse_move(current_char, pos);

            if (!move) {
                break;
            }

            pos->repetition_index++;
            pos->repetitions_table[pos->repetition_index] = pos->hash_key;
            make_move(move, all_moves, pos);

            while (*current_char && *current_char != ' ') current_char++;
            current_char++;
        }
    }
}

void parse_go(char *command, s_board *pos, s_info *info) {
    reset_time_control(info);

    int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
    info->timeset = 0;

    // init argument
    char *argument = NULL;

    // infinite search
    if ((argument = strstr(command,"infinite"))) {}

    if ((argument = strstr(command,"binc")) && pos->side == black) {
        // parse black time increment
        inc = atoi(argument + 5);
    }

    if ((argument = strstr(command,"winc")) && pos->side == white) {
        // parse white time increment
        inc = atoi(argument + 5);
    }

    if ((argument = strstr(command,"wtime")) && pos->side == white) {
        // parse white time limit
        m_time = atoi(argument + 6);
    }

    if ((argument = strstr(command,"btime")) && pos->side == black) {
        // parse black time limit
        m_time = atoi(argument + 6);
    }

    if ((argument = strstr(command,"movestogo"))) {
        // parse number of moves to go
        movestogo = atoi(argument + 10);
    }

    if ((argument = strstr(command,"movetime"))) {
        // parse amount of time allowed to spend to make a move
        movetime = atoi(argument + 9);
    }

    if ((argument = strstr(command,"depth"))) {
        depth = atoi(argument + 6);
    }

    if (movetime != -1) {
        m_time = movetime;

        info->movestogo = 1;
    }

    info->starttime = get_time_ms();

    info->depth = depth;

    // if time control is available
    if(m_time != -1)
    {
        info->timeset = 1;

        m_time /= movestogo;
        if (m_time > 1500) m_time -= 50;
        if (m_time < 0) {
            m_time = 0;
            inc -= 50;
            if (inc < 0) inc = 1;
        }
        info->stoptime = info->starttime + m_time + inc;
        // if (m_time < 1500 && inc && depth == 64) stoptime = starttime + inc - 50;
    }

    if(info->depth == -1)
        info->depth = 64;

    // search_position(depth, pos);
    main_search_thread = launch_search_thread(info->depth, pos, info);
}

void uci_loop(s_board *pos, s_info *info) {
    int max_hash = 1024;
    int mb = 64;
    int threads = 1;
    
    setbuf(stdout, NULL);

    char input[2000];

    // main loop
    while (1) {
        memset(input, 0, sizeof(input));
        fflush(stdout);

        if (!fgets(input, 2000, stdin)) { continue; }

        if (input[0] == '\n') { continue; }

        if (strncmp(input, "isready", 7) == 0) { 
            printf("readyok\n"); 
            continue;
        }

        else if (strncmp(input, "position", 8) == 0) { 
            parse_position(input, pos);
            clear_transposition_table(pos);
        }

        else if (strncmp(input, "ucinewgame", 10) == 0) { 
            parse_position("position startpos", pos); 
            engine_options->use_book = 1;
            clear_transposition_table(pos);
        }

        else if (strncmp(input, "go", 2) == 0) { 
            parse_go(input, pos, info); 
        }

        else if (strncmp(input, "stop", 4) == 0) {
            join_search_thread(info);
        }

        else if (strncmp(input, "quit", 4) == 0) {
            join_search_thread(info);
            break;
        }

        else if (strncmp(input, "uci", 3) == 0) { 
            printf("id name %s %s\n",engine_name, version);
            printf("id author Arjun Basandrai\n");
            printf("option name Hash type spin default 64 min 4 max %d\n",max_hash);
            printf("option name Threads type spin default 1 min 1 max %d\n",max_threads);
            printf("option name OwnBook type check default true\n");
            printf("uciok\n");
        }

        else if (!strncmp(input, "setoption name Hash value ", 26)) {
            sscanf(input,"%*s %*s %*s %*s %d",&mb);

            if (mb < 4) mb = 4;
            if (mb > max_hash) mb = max_hash;

            init_transposition_table(mb, pos);
        }

        else if (!strncmp(input, "setoption name Threads value ", 29)) {
            sscanf(input,"%*s %*s %*s %*s %d",&threads);

            if (threads < 1) threads = 1;
            if (threads > max_threads) threads = max_threads;
            info->threads = threads;
        }

        else if (!strncmp(input, "setoption name Book value ", 26)) {
            char *ptr = NULL;
            ptr = strstr(input, "true");
            if (ptr != NULL) {
                engine_options->use_book = 1;
            } else {
                engine_options->use_book = 0;
            }
        }
    }
}