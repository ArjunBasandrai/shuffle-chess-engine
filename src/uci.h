#pragma once

#include "board.h"
#include "search.h"

#define version "3.1.0"

void reset_time_control(s_info *info);
int parse_move(char *move_string, s_board *pos);
void parse_position(char *command, s_board *pos);
void parse_go(char *command, s_board *pos, s_info *info);
void uci_loop(s_board *pos, s_info *info);