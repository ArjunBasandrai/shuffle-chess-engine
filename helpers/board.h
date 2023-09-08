#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "helpers/bit_manipulation.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

U64 bitboards[12];
U64 occupancies[3];

int side;
int enpassant = no_sq;
int castle;

void print_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (!file) {
                printf("%d ", 8 - rank);
            }
            int piece = -1;

            for (int bitboard_piece = P; bitboard_piece <= k; bitboard_piece++) {
                if (get_bit(bitboards[bitboard_piece],square)) {
                    piece = bitboard_piece;
                }
            }
            
            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }

    printf("\n   a b c d e f g h\n\n");

    printf("   Side: %s\n", (!side) ? "white" : "black");
    printf("   Enpassant: %s\n",(enpassant != no_sq) ? sqaure_to_coordinate[enpassant] : "NO");
    printf("   Castling: %c%c%c%c\n\n",(castle & wk) ? 'K' : '-',
                                        (castle & wq) ? 'Q' : '-',
                                        (castle & bk) ? 'k' : '-',
                                        (castle & bq) ? 'q' : '-');
}
