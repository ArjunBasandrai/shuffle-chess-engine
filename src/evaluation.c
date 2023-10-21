#include "evaluation.h"

const int double_pawn_penalty = -10;
const int isolated_pawn_penalty = -10;
const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 };

const int semi_open_file_score = 10;
const int open_file_score = 15;

const int king_shield_bonus = 5;