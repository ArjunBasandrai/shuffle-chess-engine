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
    if (game_phase_score < endgame_phase_score) game_phase = endgame;
    else game_phase = middlegame;

    int score = 0;

    U64 bitboard;
    int piece, square;
    int number_of_pawns;

    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        bitboard = bitboards[bb_piece];

        while (bitboard) {
            piece = bb_piece;
            square = get_lsb_index(bitboard);

            if (game_phase == opening || game_phase == endgame) {
                score += material_score[game_phase][piece];
            } else {
                score += (
                    material_score[opening][piece] * game_phase_score + 
                    material_score[endgame][piece] * (opening_phase_score - game_phase_score)
                    ) / opening_phase_score;
            }

        /*      
        
            switch (piece) {
                case P: 
                    // positional score
                    score += pawn_score[square]; 

                    // double pawn penalty
                    number_of_pawns = count_bits(bitboards[P] & file_mask[square]);
                    if (number_of_pawns > 1) {
                        score += number_of_pawns * double_pawn_penalty;
                    }

                    // isolated pawn penalty
                    if ((bitboards[P] & isolated_mask[square]) == 0) {
                        score += isolated_pawn_penalty;
                    }

                    // passed pawn bonus
                    if ((white_passed_mask[square] & bitboards[p]) == 0) {
                        score += passed_pawn_bonus[get_rank[square]];
                    }

                    break;

                case N: score += knight_score[square]; break;
                case B: 
                    // positional score
                    score += bishop_score[square]; 

                    // mobility
                    score += count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case R: 
                    // positional score
                    score += rook_score[square];

                    // semi open file bonus
                    if ((bitboards[P] & file_mask[square]) == 0) {
                        score += semi_open_file_score;
                    }

                    // open file bonus
                    if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                        score += open_file_score;
                    }

                    break;
                
                case Q:
                    // mobility
                    score += count_bits(get_queen_attacks(square, occupancies[both]));

                case K: 
                    // posiitional score
                    score += king_score[square]; 

                    // semi open file penalty
                    if ((bitboards[P] & file_mask[square]) == 0) {
                        score -= semi_open_file_score;
                    }

                    // open file penalty
                    if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                        score -= open_file_score;
                    }

                    // king safety bonus
                    score += count_bits(king_attacks[square] & occupancies[white]) * king_shield_bonus;

                    break;

                case p: 
                    score -= pawn_score[mirror_score[square]];

                    // double pawn penalty
                    number_of_pawns = count_bits(bitboards[p] & file_mask[square]);
                    if (number_of_pawns > 1) {
                        score -= number_of_pawns * double_pawn_penalty;
                    }

                    // isolated pawn penalty
                    if ((bitboards[p] & isolated_mask[square]) == 0) {
                        score -= isolated_pawn_penalty;
                    }
                    
                    // passed pawn bonus
                    if ((black_passed_mask[square] & bitboards[P]) == 0) {
                        score -= passed_pawn_bonus[get_rank[mirror_score[square]]];
                    }

                    break;

                case n: score -= knight_score[mirror_score[square]]; break;
                case b: 
                    // positional score
                    score -= bishop_score[mirror_score[square]]; 

                    // mobility
                    score -= count_bits(get_bishop_attacks(square, occupancies[both]));

                    break;
                case r: 
                    // positional score
                    score -= rook_score[mirror_score[square]]; 

                    // semi open file bonus
                    if ((bitboards[p] & file_mask[square]) == 0) {
                        score -= semi_open_file_score;
                    }

                    // open file bonus
                    if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                        score -= open_file_score;
                    }

                    break;
                
                case q:
                    // mobility
                    score -= count_bits(get_queen_attacks(square, occupancies[both]));

                case k: 
                    // positional score
                    score -= king_score[mirror_score[square]]; 

                    // semi open file penalty
                    if ((bitboards[p] & file_mask[square]) == 0) {
                        score += semi_open_file_score;
                    }

                    // open file penalty
                    if (((bitboards[P] | bitboards[p]) & file_mask[square]) == 0) {
                        score += open_file_score;
                    }

                    // king safety bonus
                    score -= count_bits(king_attacks[square] & occupancies[black]) * king_shield_bonus;
                    
                    break;
            }
        */

            pop_bit(bitboard, square);
        }
    }

    return (side == white) ? score : -score;
}