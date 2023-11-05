#include "evaluation.h"

const int double_pawn_penalty[2] = {-5, -10};
const int isolated_pawn_penalty[2][8] = {
  {14, 21, 23, 23, 23, 23, 21, 14},
  { 5,  7,  8,  8,  8,  8,  7,  5}
};

const int connected_pawn_bonus[2][64] = {
  { 0,  0,  0,  0,  0,  0,  0,  0,
   54, 63, 65, 70, 70, 65, 63, 54,
   27, 29, 31, 35, 35, 31, 29, 27,
   12, 14, 17, 22, 22, 17, 14, 12,
    3,  5,  6, 10, 10,  6,  5,  3,
    1,  2,  2,  3,  3,  2,  2,  1,
    0,  1,  1,  2,  2,  1,  1,  0,
    0,  0,  0,  0,  0,  0,  0,  0},
  
  { 0,  0,  0,  0,  0,  0,  0,  0,
   76, 87, 87, 87, 87, 87, 87, 76,
   32, 38, 38, 38, 38, 38, 38, 32,
   13, 17, 17, 17, 17, 17, 17, 13,
    6, 10, 10, 10, 10, 10, 10,  3,
    3,  6,  6,  6,  6,  6,  6,  1,
    1,  3,  3,  3,  3,  3,  1,  1,
    0,  0,  0,  0,  0,  0,  0,  0}
};

const int backward_pawn_penalty[2][8] = {
  { 8, 12, 14, 14, 14, 14, 12, 8 },
  { 2,  3,  3,  3,  3,  3,  3, 2 }
};

const int passed_pawn[8] = { 0, 0, 0, 2, 6, 12, 21, 0 };
const int passed_pawn_base[2] = { 4, 8 };
const int passed_pawn_free_advance = 8;
const int passed_pawn_partial_advance = 5;
const int passed_pawn_defended = 7;
const int passed_pawn_partial_defended = 4;

const int rook_semi_open_file_score = 10;
const int rook_open_file_score[2] = {5, 20};

const int king_open_file_penalty[8] = {6, 5, 4, 4, 4, 4, 5, 6};
const int king_semi_open_file_penalty[8] = {4, 4, 3, 3, 3, 3, 4, 4};

const int pawn_defects[2][8] = {
  { 0, 0, 3, 2, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 2, 3, 0, 0 }
};

const int king_shield_bonus = 5;