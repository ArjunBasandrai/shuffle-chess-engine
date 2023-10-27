#pragma once

#include "board_constants.h"
#include "bit_manipulation.h"
#include "board.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern const int double_pawn_penalty;
extern const int isolated_pawn_penalty;
extern const int passed_pawn_bonus[8];

extern const int semi_open_file_score;
extern const int open_file_score;

extern const int king_shield_bonus;

static inline int get_game_phase_score() {
    int white_piece_scores=0, black_piece_scores=0;

    for (int piece = N; piece <= Q; piece++) {
        white_piece_scores += count_bits(bitboards[piece]) * material_score[opening][piece];
    }

    for (int piece = n; piece <= q; piece++) {
        black_piece_scores += count_bits(bitboards[piece]) * -material_score[opening][piece];
    }

    return white_piece_scores + black_piece_scores;
}

static inline int evaluate() {

    int game_phase_score = get_game_phase_score();
    int game_phase = -1;


    if (game_phase_score > opening_phase_score) game_phase = opening;
    else if (game_phase_score < endgame_phase_score) game_phase = endgame;
    else game_phase = middlegame;

    int score = 0, score_opening = 0, score_endgame = 0;

    U64 bitboard;
    int piece, square;
    int number_of_pawns;

    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        bitboard = bitboards[bb_piece];

        while (bitboard) {
            piece = bb_piece;
            square = get_lsb_index(bitboard);

            score_opening += material_score[opening][piece];
            score_endgame += material_score[endgame][piece];
        
            switch (piece) {
                case P: 
                    // positional score
                    score_opening += positional_score[opening][PAWN][square];
                    score_endgame += positional_score[endgame][PAWN][square];

                    // double pawn penalty
                    // number_of_pawns = count_bits(bitboards[P] & file_mask[square]);
                    // if (number_of_pawns > 1) {
                    //     score += number_of_pawns * double_pawn_penalty;
                    // }

                    // isolated pawn penalty
                    // if ((bitboards[P] & isolated_mask[square]) == 0) {
                    //     score += isolated_pawn_penalty;
                    // }

                    // passed pawn bonus
                    // if ((white_passed_mask[square] & bitboards[p]) == 0) {
                    //     score += passed_pawn_bonus[get_rank[square]];
                    // }

                    break;

                case N:
                    // positional score
                    score_opening += positional_score[opening][KNIGHT][square];
                    score_endgame += positional_score[endgame][KNIGHT][square]; 

                    break;
                case B: 
                    // positional score
                    score_opening += positional_score[opening][BISHOP][square];
                    score_endgame += positional_score[endgame][BISHOP][square]; 

                    // mobility
                    // score += count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case R: 
                    // positional score
                    score_opening += positional_score[opening][ROOK][square];
                    score_endgame += positional_score[endgame][ROOK][square]; 

                    // semi open file bonus
                    // if ((bitboards[P] & file_mask[square]) == 0) {
                    //     score += semi_open_file_score;
                    // }

                    // open file bonus
                    // if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                    //     score += open_file_score;
                    // }

                    break;
                
                case Q:
                    // positional score
                    score_opening += positional_score[opening][QUEEN][square];
                    score_endgame += positional_score[endgame][QUEEN][square]; 

                    break;

                case K: 
                    // positional score
                    score_opening += positional_score[opening][KING][square];
                    score_endgame += positional_score[endgame][KING][square]; 

                    // semi open file penalty
                    // if ((bitboards[P] & file_mask[square]) == 0) {
                    //     score -= semi_open_file_score;
                    // }

                    // open file penalty
                    // if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                    //     score -= open_file_score;
                    // }

                    // king safety bonus
                    // score += count_bits(king_attacks[square] & occupancies[white]) * king_shield_bonus;

                    break;

                case p: 
                    // positional score
                    score_opening -= positional_score[opening][PAWN][mirror_score[square]];
                    score_endgame -= positional_score[endgame][PAWN][mirror_score[square]]; 

                    // double pawn penalty
                    // number_of_pawns = count_bits(bitboards[p] & file_mask[square]);
                    // if (number_of_pawns > 1) {
                    //     score -= number_of_pawns * double_pawn_penalty;
                    // }

                    // isolated pawn penalty
                    // if ((bitboards[p] & isolated_mask[square]) == 0) {
                    //     score -= isolated_pawn_penalty;
                    // }
                    
                    // passed pawn bonus
                    // if ((black_passed_mask[square] & bitboards[P]) == 0) {
                    //     score -= passed_pawn_bonus[get_rank[mirror_score[square]]];
                    // }

                    break;
                
                case n: 
                    // positional score
                    score_opening -= positional_score[opening][KNIGHT][mirror_score[square]];
                    score_endgame -= positional_score[endgame][KNIGHT][mirror_score[square]];

                    break;
                
                case b: 
                    // positional score
                    score_opening -= positional_score[opening][BISHOP][mirror_score[square]];
                    score_endgame -= positional_score[endgame][BISHOP][mirror_score[square]];

                    // mobility
                    // score -= count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case r: 
                    // positional score
                    score_opening -= positional_score[opening][ROOK][mirror_score[square]];
                    score_endgame -= positional_score[endgame][ROOK][mirror_score[square]];

                    // semi open file bonus
                    // if ((bitboards[p] & file_mask[square]) == 0) {
                    //     score -= semi_open_file_score;
                    // }

                    // open file bonus
                    // if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                    //     score -= open_file_score;
                    // }

                    break;
                
                case q:
                    // positional score
                    score_opening -= positional_score[opening][QUEEN][mirror_score[square]];
                    score_endgame -= positional_score[endgame][QUEEN][mirror_score[square]];

                    break;

                case k: 
                    // positional score
                    score_opening -= positional_score[opening][KING][mirror_score[square]];
                    score_endgame -= positional_score[endgame][KING][mirror_score[square]];

                    // semi open file penalty
                    // if ((bitboards[p] & file_mask[square]) == 0) {
                    //     score += semi_open_file_score;
                    // }

                    // open file penalty
                    // if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                    //     score += open_file_score;
                    // }

                    // king safety bonus
                    // score -= count_bits(king_attacks[square] & occupancies[black]) * king_shield_bonus;
                    
                    break;
            }
        
            pop_bit(bitboard, square);
        }
    }

    if (game_phase == middlegame) {
        score = (
            score_opening * game_phase_score + 
            score_endgame * (opening_phase_score - game_phase_score)
        ) / opening_phase_score;
    } else if (game_phase == opening) {
        score = score_opening;
    } else if (game_phase == endgame) {
        score = score_endgame;
    }

    score = score * (100 - fifty) / 100;

    return (side == white) ? score : -score;
}