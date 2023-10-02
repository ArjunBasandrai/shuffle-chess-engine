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

static inline int evaluate() {
    int score = 0;

    U64 bitboard;
    int piece, square;
    int number_of_pawns;

    for (int bb_piece = P; bb_piece <= k; bb_piece++) {
        bitboard = bitboards[bb_piece];

        while (bitboard) {
            piece = bb_piece;
            square = get_lsb_index(bitboard);

            score += material_score[piece];
            
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
                case B: score += bishop_score[square]; break;
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
                case b: score -= bishop_score[mirror_score[square]]; break;
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
                    
                    break;
            }

            pop_bit(bitboard, square);
        }
    }

    return (side == white) ? score : -score;
}