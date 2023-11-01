#pragma once

#include "board.h"

#define version "1.8.0"

void reset_time_control();
int parse_move(char *move_string, s_board *pos);
void parse_position(char *command, s_board *pos);
void parse_go(char *command, s_board *pos);
void uci_loop(s_board *pos);