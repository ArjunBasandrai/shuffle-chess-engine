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
#include "helpers/board_constants.h"
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "helpers/moves_list.h"
#endif

#ifndef MOVEGEN_H_
#define MOVEGEN_H_
#include "helpers/movegen.h"
#endif

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

}