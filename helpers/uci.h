#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef STRING_H_
#define STRING_H_
#include <string.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "moves_list.h"
#endif

#ifndef MOVEGEN_H_
#define MOVEGEN_H_
#include "movegen.h"
#endif

#ifndef SEARCH_H_
#define SEARCH_H_
#include "search.h"
#endif

#ifndef FEN_H_
#define FEN_H_
#include "fen.h"
#endif

#include "stdlib.h"

int parse_move(char *move_string) {
    moves move_list[1];
    generate_moves(move_list);

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

    printf("source square: %s\n",sqaure_to_coordinate[target_square]);
}


void parse_position(char *command) {
    command += 9;
    char *current_char = command;

    // parse UCI startpos command
    if (strncmp(command, "startpos", 8) == 0) {
        parse_fen(start_position);
    } 
    
    // parse UCI fen command
    else {
        current_char = strstr(command, "fen");

        if (current_char == NULL) {
            parse_fen(start_position);
        }

        else {
            current_char += 4;

            parse_fen(current_char);
        }
    }

    current_char = strstr(command,"moves");

    if (current_char != NULL) {

        current_char += 6;

        while(*current_char) {
            int move = parse_move(current_char);

            if (!move) {
                break;
            }

            make_move(move, all_moves);

            while (*current_char && *current_char != ' ') current_char++;
            current_char++;
        }
    }
    print_board();
}

void parse_go(char *command) {
    int depth = -1;
    char *current_depth = NULL;

    if (current_depth = strstr(command, "depth")) {
        depth = atoi(current_depth + 6);
    } else {
        depth = 6;
    }

    search_position(depth);
}

void uci_loop() {
    // clear STDIN & STDOUT buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char input[2000];

    printf("id name %s\n",engine_name);
    printf("id name Arjun Basandrai\n");
    printf("uciok\n");

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
            parse_position(input); 
        }

        else if (strncmp(input, "ucinewgame", 10) == 0) { 
            parse_position("position startpos"); 
        }

        else if (strncmp(input, "go", 2) == 0) { 
            parse_go(input); 
        }

        else if (strncmp(input, "quit", 4) == 0) { 
            break;
        }

        else if (strncmp(input, "uci", 3) == 0) { 
            printf("id name %s\n",engine_name);
            printf("id name Arjun Basandrai\n");
            printf("uciok\n");
        }
    }
}