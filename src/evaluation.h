#pragma once

#include "board_constants.h"
#include "bit_manipulation.h"
#include "pre_calculated_tables.h"
#include "board.h"

#ifndef U64
#define U64 unsigned long long
#endif

extern const int double_pawn_penalty[2];
extern const int isolated_pawn_penalty[2];
extern const int passed_pawn_bonus[8];

extern const int semi_open_file_score;
extern const int open_file_score;

static const int bishop_unit = 4;
static const int queen_unit = 9;

static const int bishop_mobility[2] = {5, 6};
static const int queen_mobility[2] = {1, 2};

extern const int king_shield_bonus;

static inline int get_game_phase_score(s_board *pos) {
    int white_piece_scores=0, black_piece_scores=0;

    for (int piece = N; piece <= Q; piece++) {
        white_piece_scores += count_bits(pos->bitboards[piece]) * material_score[opening][piece];
    }

    for (int piece = n; piece <= q; piece++) {
        black_piece_scores += count_bits(pos->bitboards[piece]) * -material_score[opening][piece];
    }

    return white_piece_scores + black_piece_scores;
}

static inline int evaluate(s_board *pos) {

    int game_phase_score = get_game_phase_score(pos);
    int game_phase = -1;


    if (game_phase_score > opening_phase_score) game_phase = opening;
    else if (game_phase_score < endgame_phase_score) game_phase = endgame;
    else game_phase = middlegame;

    int score = 0, score_opening = 0, score_endgame = 0;

    U64 bitboard;
    int piece, square;

    int double_pawns = 0;

    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        bitboard = pos->bitboards[bb_piece];

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
                    double_pawns = count_bits(pos->bitboards[P] & file_mask[square]);
                    if (double_pawns > 1) {
                        score_opening += (double_pawns - 1) * double_pawn_penalty[opening];
                        score_endgame += (double_pawns - 1) * double_pawn_penalty[endgame];
                    }
                    
                    // isolated pawn penalty
                    if ((pos->bitboards[P] & isolated_mask[square]) == 0) {
                        score_opening += isolated_pawn_penalty[opening];
                        score_endgame += isolated_pawn_penalty[endgame];
                    }

                    // passed pawn bonus
                    if ((white_passed_mask[square] & pos->bitboards[p]) == 0) {
                        score_opening += passed_pawn_bonus[get_rank[square]];
                        score_endgame += passed_pawn_bonus[get_rank[square]];
                    }

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

                    // mobility score
                    score_opening += (count_bits(get_bishop_attacks(square, pos->occupancies[both])) - bishop_unit) * bishop_mobility[opening];
                    score_endgame += (count_bits(get_bishop_attacks(square, pos->occupancies[both])) - bishop_unit) * bishop_mobility[endgame]; 

                    break;
                case R: 
                    // positional score
                    score_opening += positional_score[opening][ROOK][square];
                    score_endgame += positional_score[endgame][ROOK][square]; 

                    // semi open file bonus
                    if ((pos->bitboards[P] & file_mask[square]) == 0) {
                        // add semi open file bonus
                        score_opening += semi_open_file_score;
                        score_endgame += semi_open_file_score;
                    }

                    // open file bonus
                    if (((pos->bitboards[P] | pos->bitboards[p]) & file_mask[square]) == 0) {
                        // add semi open file bonus
                        score_opening += open_file_score;
                        score_endgame += open_file_score;
                    }

                    break;
                
                case Q:
                    // positional score
                    score_opening += positional_score[opening][QUEEN][square];
                    score_endgame += positional_score[endgame][QUEEN][square]; 

                    // mobility score
                    score_opening += (count_bits(get_queen_attacks(square, pos->occupancies[both])) - queen_unit) * queen_mobility[opening];
                    score_endgame += (count_bits(get_queen_attacks(square, pos->occupancies[both])) - queen_unit) * queen_mobility[endgame]; 

                    break;

                case K: 
                    // positional score
                    score_opening += positional_score[opening][KING][square];
                    score_endgame += positional_score[endgame][KING][square]; 

                    // semi open file penalty
                    if ((pos->bitboards[p] & file_mask[square]) == 0) {
                        score_opening += semi_open_file_score;
                        score_endgame += semi_open_file_score;
                    }

                    // open file penalty
                    if (((pos->bitboards[P] | pos->bitboards[p]) & file_mask[square]) == 0) {
                        score_opening += open_file_score;
                        score_endgame += open_file_score;
                    }

                    // king safety bonus
                    score_opening += count_bits(king_attacks[square] & pos->occupancies[black]) * king_shield_bonus;
                    score_endgame += count_bits(king_attacks[square] & pos->occupancies[black]) * king_shield_bonus;

                    break;

                case p: 
                    // positional score
                    score_opening -= positional_score[opening][PAWN][mirror_score[square]];
                    score_endgame -= positional_score[endgame][PAWN][mirror_score[square]]; 

                    // double pawn penalty
                    double_pawns = count_bits(pos->bitboards[p] & file_mask[square]);
                    if (double_pawns > 1) {
                        score_opening -= (double_pawns - 1) * double_pawn_penalty[opening];
                        score_endgame -= (double_pawns - 1) * double_pawn_penalty[endgame];
                    }

                    // isolated pawn penalty
                    if ((pos->bitboards[p] & isolated_mask[square]) == 0) {
                        score_opening -= isolated_pawn_penalty[opening];
                        score_endgame -= isolated_pawn_penalty[endgame];
                    }
                    
                    // passed pawn bonus
                    if ((black_passed_mask[square] & pos->bitboards[P]) == 0) {
                        score_opening -= passed_pawn_bonus[get_rank[mirror_score[square]]];
                        score_endgame -= passed_pawn_bonus[get_rank[mirror_score[square]]];
                    }

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

                    // mobility score
                    score_opening -= (count_bits(get_bishop_attacks(square, pos->occupancies[both])) - bishop_unit) * bishop_mobility[opening];
                    score_endgame -= (count_bits(get_bishop_attacks(square, pos->occupancies[both])) - bishop_unit) * bishop_mobility[endgame];

                    break;
                case r: 
                    // positional score
                    score_opening -= positional_score[opening][ROOK][mirror_score[square]];
                    score_endgame -= positional_score[endgame][ROOK][mirror_score[square]];

                    // semi open file bonus
                    if ((pos->bitboards[p] & file_mask[square]) == 0) {
                        // add semi open file bonus
                        score_opening -= semi_open_file_score;
                        score_endgame -= semi_open_file_score;
                    }

                    // open file bonus
                    if (((pos->bitboards[P] | pos->bitboards[p]) & file_mask[square]) == 0) {    
                        // add semi open file bonus
                        score_opening -= open_file_score;
                        score_endgame -= open_file_score;
                    }

                    break;
                
                case q:
                    // positional score
                    score_opening -= positional_score[opening][QUEEN][mirror_score[square]];
                    score_endgame -= positional_score[endgame][QUEEN][mirror_score[square]];

                    // mobility score
                    score_opening -= (count_bits(get_queen_attacks(square, pos->occupancies[both])) - queen_unit) * queen_mobility[opening];
                    score_endgame -= (count_bits(get_queen_attacks(square, pos->occupancies[both])) - queen_unit) * queen_mobility[endgame]; 

                    break;

                case k: 
                    // positional score
                    score_opening -= positional_score[opening][KING][mirror_score[square]];
                    score_endgame -= positional_score[endgame][KING][mirror_score[square]];

                    // semi open file penalty
                    if ((pos->bitboards[P] & file_mask[square]) == 0) {
                        score_opening -= semi_open_file_score;
                        score_endgame -= semi_open_file_score;
                    }

                    // open file penalty
                    if (((pos->bitboards[P] | pos->bitboards[p]) & file_mask[square]) == 0) {
                        score_opening -= open_file_score;
                        score_endgame -= open_file_score;
                    }

                    // king safety bonus
                    score_opening -= count_bits(king_attacks[square] & pos->occupancies[black]) * king_shield_bonus;
                    score_endgame -= count_bits(king_attacks[square] & pos->occupancies[black]) * king_shield_bonus;
                    
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

    score = score * (100 - pos->fifty) / 100;

    return (pos->side == white) ? score : -score;
}