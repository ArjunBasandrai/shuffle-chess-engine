//system headers
#include <stdio.h>

// define bitboard data type
#define U64 unsigned long long

// bit macros
#define set_bit(bitboard,square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard,square) (get_bit(bitboard,square) ? bitboard ^= (1ULL << square) : 0)

// enum board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// Bit manipulations
void print_bitboard(U64 bitboard) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if (!file){
                printf(" %d ",8 - rank);
            }
            printf(" %d",get_bit(bitboard,square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n\n");
    printf("    Bitboard: %llud\n\n",bitboard);
}

// Main driver
int main(){
    U64 bitboard = 0ULL;

    set_bit(bitboard,e4);
    set_bit(bitboard,a1);
    set_bit(bitboard,a2);
    
    pop_bit(bitboard,a1);
    
    print_bitboard(bitboard);
    return 0;
}