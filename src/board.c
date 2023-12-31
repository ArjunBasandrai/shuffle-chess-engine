#include <stdio.h>

#include "board.h"

#include "bit_manipulation.h"

#ifndef U64
#define U64 unsigned long long
#endif

void print_board(s_board *pos) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (!file) {
                printf("%d ", 8 - rank);
            }
            int piece = -1;

            for (int bitboard_piece = P; bitboard_piece <= k; bitboard_piece++) {
                if (get_bit(pos->bitboards[bitboard_piece],square)) {
                    piece = bitboard_piece;
                }
            }
            
            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }

    printf("\n   a b c d e f g h\n\n");

    printf("   Side: %s\n", (!pos->side) ? "white" : "black");
    printf("   Enpassant: %s\n",(pos->enpassant != no_sq) ? square_to_coordinate[pos->enpassant] : "NO");
    printf("   Castling: %c%c%c%c\n",(pos->castle & wk) ? 'K' : '-',
                                        (pos->castle & wq) ? 'Q' : '-',
                                        (pos->castle & bk) ? 'k' : '-',
                                        (pos->castle & bq) ? 'q' : '-');
    printf("   Hash Key: %llx\n\n", pos->hash_key);
    printf("   Fifty move: %d\n\n", pos->fifty);
}
