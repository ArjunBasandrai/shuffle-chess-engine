#include "evaluation.h"

// Pawn Evaluation Constants
int doubled_pawn_penalty[2][8] = {
    {14, 16, 18, 14, 18, 18, 16, 10, },
    {44, 38, 34, 35, 38, 34, 38, 44, },
};

int isolated_pawn_penalty[2][8] = {
    {14, 21, 23, 23, 23, 23, 21, 14, },
    {5, 7, 8, 8, 8, 4, 7, 5, },
};

int backward_pawn_penalty[2][8] = {
    {4, 8, 10, 10, 10, 10, 8, 4, },
    {0, 0, 0, 0, 0, 0, 0, 0, },
};

int pawn_defects[2][8] = {
    {0, 0, 3, 2, 1, 0, 0, 0, },
    {0, 0, 0, 1, 2, 3, 0, 0, },
};

int unsupported_pawn_penalty[2] = {4, 4};

int connected_pawn_bonus[2][64] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 
        50, 59, 61, 66, 66, 61, 59, 50, 
        23, 25, 27, 31, 31, 27, 25, 23, 
        8, 10, 13, 18, 18, 13, 10, 8, 
        0, 1, 2, 6, 6, 2, 1, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
    {0, 0, 0, 0, 0, 0, 0, 0, 
        72, 83, 83, 83, 83, 83, 83, 72, 
        28, 34, 34, 34, 34, 34, 34, 28, 
        9, 13, 13, 13, 13, 13, 13, 9, 
        2, 6, 6, 6, 6, 6, 6, 0, 
        0, 2, 2, 2, 2, 2, 2, 0, 
        0, 0, 0, 3, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
};

int passed_pawn[8] = {0, 0, 0, 0, 2, 8, 17, 0, };

int passed_pawn_base[2] = {0, 4};

int passed_pawn_free_advance = 4;

int passed_pawn_partial_advance = 1;

int passed_pawn_defended = 3;

int passed_pawn_partial_defended = 0;

// Knight Evaluation Constants
int knight_outpost[2][64] = {
    {   0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 2, 2, 2, 2, 1, 0, 
        0, 2, 4, 4, 4, 4, 2, 0, 
        0, 3, 2, 8, 4, 6, 3, 0, 
        0, 0, 4, 0, 4, 4, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
    {   0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 2, 0, 4, 0, 0, 0, 0, 
        0, 3, 3, 4, 8, 2, 3, 0, 
        0, 2, 0, 4, 3, 0, 2, 0, 
        0, 0, 2, 0, 2, 0, 1, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
};

// Bishop Evaluation Constants
int bishop_outpost[2][64] = {
    {   0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 1, 0, 0, 
        0, 1, 0, 0, 0, 0, 1, 0, 
        0, 0, 1, 1, 1, 1, 3, 0, 
        0, 0, 0, 0, 0, 0, 1, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
    {   0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 1, 0, 0, 0, 0, 1, 0, 
        0, 0, 1, 1, 1, 1, 3, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 
        },
};

int bishop_pawns_on_color[2] = {0, 6};

// Rook Evaluation Constants
int rook_semi_open_file_score = 10;

int rook_open_file_score[2] = {5, 20};

// King Evaluation Constants
int king_open_file_penalty[8] = {6, 5, 4, 4, 4, 4, 5, 6, };

int king_semi_open_file_penalty[8] = {4, 0, 0, 3, 0, 0, 0, 0, };

int king_shield_bonus = 5;


// Mobility Evaluation Constants
int bishop_mobility[2] = {1, 2};

int queen_mobility[2] = {1, 2};

int bishop_unit = 4;

int queen_unit = 5;
