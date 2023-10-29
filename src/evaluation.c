#include "evaluation.h"

const int double_pawn_penalty[2] = {-5, -10};
const int isolated_pawn_penalty[2] = {-5, -10};
const int passed_pawn_bonus[8] = { 
    0, 5, 15, 25, 50, 75, 125, 175,
};

const int semi_open_file_score = 10;
const int open_file_score = 15;

const int king_shield_bonus = 5;