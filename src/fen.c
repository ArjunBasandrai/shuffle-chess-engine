#include <string.h>

#include "fen.h"

#include "bit_manipulation.h"
#include "board_constants.h"
#include "zobrist.h"

void parse_fen(char *fen, s_board *pos) {
    memset(pos->bitboards, 0ULL,sizeof(pos->bitboards));
    memset(pos->occupancies,0ULL,sizeof(pos->occupancies));
    pos->side = 0;
    pos->enpassant = no_sq;
    pos->castle = 0;
    pos->hash_key = 0ULL;
    repetition_index = 0;
    memset(repetitions_table, 0ULL, sizeof(repetitions_table));
    ply = 0;
    pos->fifty = 0;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece  = char_pieces[*fen];

                set_bit(pos->bitboards[piece],square);

                fen++;
            }
            if (*fen >= '0' && *fen <= '9') {
                int offset = *fen - '0';

                int piece = -1;
                for (int bb_piece = P; bb_piece <= k; bb_piece++) {
                    if (get_bit(pos->bitboards[bb_piece],square)) {
                        piece = bb_piece;
                    }
                }
                if (piece == -1) {
                    file--;
                }
                file+=offset;
                fen++;
            }
            if (*fen == '/') {
                fen++;
            }
        }
    }
    fen++;
    pos->side = (*fen=='w') ? white : black;
    fen += 2;

    while (*fen != ' ') {
        switch (*fen) {
            case 'K':
            pos->castle |= wk;
            break;

            case 'Q':
            pos->castle |= wq;
            break;

            case 'k':
            pos->castle |= bk;
            break;

            case 'q':
            pos->castle |= bq;
            break;

            case '-':
            break;
        }
        fen++;
    }

    fen++;
    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        pos->enpassant = rank * 8 + file; 
    } else {
        pos->enpassant = no_sq;
    }

    for (int piece = P; piece <= K; piece++) {
        pos->occupancies[white] |= pos->bitboards[piece];
    }

    for (int piece = p; piece <= k; piece++) {
        pos->occupancies[black] |= pos->bitboards[piece];
    }

    pos->occupancies[both] |= pos->occupancies[white];
    pos->occupancies[both] |= pos->occupancies[black];
    
    pos->hash_key = generate_hash_keys(pos);
}
