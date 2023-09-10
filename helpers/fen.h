#ifndef STRING_H_
#define STRING_H_
#include <string.h>
#endif

#ifndef BITS_H_
#define BITS_H_
#include "bit_manipulation.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "board.h"
#endif

#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define custom_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

void parse_fen(char *fen) {
    memset(bitboards, 0ULL,sizeof(bitboards));
    memset(occupancies,0ULL,sizeof(occupancies));
    side = 0;
    enpassant = no_sq;
    castle = 0;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece  = char_pieces[*fen];

                set_bit(bitboards[piece],square);

                fen++;
            }
            if (*fen >= '0' && *fen <= '9') {
                int offset = *fen - '0';

                int piece = -1;
                for (int bb_piece = P; bb_piece <= k; bb_piece++) {
                    if (get_bit(bitboards[bb_piece],square)) {
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
    side = (*fen=='w') ? white : black;
    fen += 2;

    while (*fen != ' ') {
        switch (*fen) {
            case 'K':
            castle |= wk;
            break;

            case 'Q':
            castle |= wq;
            break;

            case 'k':
            castle |= bk;
            break;

            case 'q':
            castle |= bq;
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

        enpassant = rank * 8 + file; 
    } else {
        enpassant = no_sq;
    }

    for (int piece = P; piece <= K; piece++) {
        occupancies[white] |= bitboards[piece];
    }

    for (int piece = p; piece <= k; piece++) {
        occupancies[black] |= bitboards[piece];
    }

    occupancies[both] |= occupancies[white];
    occupancies[both] |= occupancies[black];
}
