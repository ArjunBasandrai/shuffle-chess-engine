#include "masks.h"

#include "bit_manipulation.h"
#include "board_constants.h"

U64 mask_pawn_attacks(int square, int side) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard,square);

    if (!side) {
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    }

    else {
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }

    return attacks;
}

U64 mask_knight_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard,square);

    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);

    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

    return attacks;
}

U64 mask_king_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard,square);

    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);

    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);

    return attacks;
}

U64 mask_bishop_attacks(int square){
    U64 attacks = 0ULL;

    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

U64 mask_rook_attacks(int square){
    U64 attacks = 0ULL;

    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

U64 bishop_attacks_on_the_fly(int square,U64 block){
    U64 attacks = 0ULL;

    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    return attacks;
}

U64 rook_attacks_on_the_fly(int square, U64 block){
    U64 attacks = 0ULL;

    int r, f;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    return attacks;
}

U64 get_attackers(const s_board *pos, int square) {
    U64 attacks = 0ULL;

    attacks |= (mask_pawn_attacks(square, black) & pos->bitboards[P]) | (mask_pawn_attacks(square, white) & pos->bitboards[p]);
    attacks |= (mask_knight_attacks(square) & (pos->bitboards[N] | pos->bitboards[n]));
    attacks |= (mask_king_attacks(square) & (pos->bitboards[K] | pos->bitboards[k]));
    attacks |= (bishop_attacks_on_the_fly(square,pos->occupancies[both]) & (pos->bitboards[B] | pos->bitboards[b] | pos->bitboards[Q] | pos->bitboards[q]));
    attacks |= (rook_attacks_on_the_fly(square, pos->occupancies[both]) & (pos->bitboards[R] | pos->bitboards[r] | pos->bitboards[Q] | pos->bitboards[q]));

    return attacks;
}

U64 set_file_rank_mask(int file_number, int rank_number) {
    U64 mask = 0ULL;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (file_number != -1) {
                if (file == file_number) {
                    mask |= set_bit(mask, square);
                }
            } else if (rank_number != -1) {
                if (rank == rank_number) {
                    mask |= set_bit(mask, square);
                }
            }
        }
    }

    return mask;
}

void init_evaluation_masks() {

    // init file masks

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            file_mask[square] |= set_file_rank_mask(file, -1);
        }
    }

    // init rank masks

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            rank_mask[square] |= set_file_rank_mask(-1, rank);
        }
    }

    // init isolated masks

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            isolated_mask[square] |= set_file_rank_mask(file - 1, -1);
            isolated_mask[square] |= set_file_rank_mask(file + 1, -1);
        }
    }

    // init connected masks

    for (int rank = 1; rank < 7; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (file > 0 && file < 7) {
                connected_mask[white][square] |= ((file_mask[square - 1] | file_mask[square + 1]) & (rank_mask[square] | rank_mask[square + 8]));
                connected_mask[black][square] |= ((file_mask[square - 1] | file_mask[square + 1]) & (rank_mask[square] | rank_mask[square - 8]));
            } else if (file == 0) {
                connected_mask[white][square] |= (file_mask[square + 1] & (rank_mask[square] | rank_mask[square + 8]));
                connected_mask[black][square] |= (file_mask[square + 1] & (rank_mask[square] | rank_mask[square - 8]));
            } else if (file == 7) {
                connected_mask[white][square] |= (file_mask[square - 1] & (rank_mask[square] | rank_mask[square + 8]));
                connected_mask[black][square] |= (file_mask[square - 1] & (rank_mask[square] | rank_mask[square - 8]));
            }
        }
    }

    // init file ahead and behind masks

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            int square = rank * 8 + file;
            if (rank > 0) {
                for (int i = rank - 1; i >= 0; i--) {
                    file_ahead_mask[square] |= rank_mask[i * 8 + file];
                }
            }
            if (rank < 7) {
                for (int i = rank + 1; i < 8; i++) {
                    file_behind_mask[square] |= rank_mask[i * 8 + file];
                }
            }
            file_ahead_mask[square] &= file_mask[square];
            file_behind_mask[square] &= file_mask[square];
        }
    }

    // init white passed pawn masks

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            white_passed_mask[square] |= set_file_rank_mask(file - 1, -1);
            white_passed_mask[square] |= set_file_rank_mask(file, -1);
            white_passed_mask[square] |= set_file_rank_mask(file + 1, -1);

            for (int i=0; i < (7 - rank) + 1; i++) {
                white_passed_mask[square] &= ~rank_mask[(7 - i) * 8 + file];
            }
        }
    }

    // init white passed pawn masks

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            black_passed_mask[square] |= set_file_rank_mask(file - 1, -1);
            black_passed_mask[square] |= set_file_rank_mask(file, -1);
            black_passed_mask[square] |= set_file_rank_mask(file + 1, -1);

            for (int i=0; i < rank + 1; i++) {
                black_passed_mask[square] &= ~rank_mask[i * 8 + file];
            }
        }
    }
}