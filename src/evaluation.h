#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "bit_manipulation.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "board.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

const int double_pawn_penalty = -10;
const int isolated_pawn_penalty = -10;
const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 };

const int semi_open_file_score = 10;
const int open_file_score = 15;

const int king_shield_bonus = 5;

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

                    break;
                case B: 

                    // mobility
                    // score += count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case R: 

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

                    break;

                case K: 

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

                    break;
                
                case b: 

                    // mobility
                    // score -= count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case r: 

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

                    break;

                case k: 

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

    // if (game_phase == middlegame) {
    //     score += (
    //         material_score[opening][piece] * game_phase_score + 
    //         material_score[endgame][piece] * (opening_phase_score - game_phase_score)
    //     ) / opening_phase_score;
    // } else {
    //     score += material_score[game_phase][piece];
    // }

    if (game_phase == middlegame) {
        printf("middlegame\n");
    } else if (game_phase == opening) {
        score = score_opening;
    } else if (game_phase == endgame) {
        score = score_endgame;
    }

    return (side == white) ? score : -score;
}