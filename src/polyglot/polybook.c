#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../bit_manipulation.h"
#include "../board_constants.h"
#include "../board.h"
#include "polykeys.h"

typedef struct {
    U64 key;
    unsigned short move;
    unsigned short weight;
    unsigned int learn;
} s_polyglot_book_entry;

long num_entries = 0;

s_polyglot_book_entry *entries;

const int polyglot_pieces[12] = {
    1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

void init_poly_book() {
    FILE *pFile = fopen("src/polyglot/polyglot_opening_books/shuffle.bin", "rb");
    if (pFile == NULL) {
        perror("No PolyGlot book found");
        return;
    } else {
        fseek(pFile, 0, SEEK_END);
        long position = ftell(pFile);

        if (position < sizeof(s_polyglot_book_entry)) {
            perror("Empty PolyGlot book");
            return;
        }

        num_entries = position / sizeof(s_polyglot_book_entry);
        // printf("%ld entries found\n", num_entries);

        entries = (s_polyglot_book_entry*)malloc(num_entries * sizeof(s_polyglot_book_entry));

        rewind(pFile);

        size_t return_value;
        return_value = fread(entries, sizeof(s_polyglot_book_entry), num_entries, pFile);
        // printf("fread() %ld entries read in from file\n", return_value);
    }
}

void clean_poly_book() {
    free(entries);
}

int has_enpassant_capture() {
    int sq_with_pawn = 0;
    int target_piece = (side == white) ? P : p;
    if (enpassant != no_sq) {
        if (side == white) {
            sq_with_pawn = enpassant + 8;
        } else {
            sq_with_pawn = enpassant - 8;
        }

        U64 bitboard = bitboards[target_piece];

        if (get_bit(bitboard, sq_with_pawn + 1)) return 1;
        else if (get_bit(bitboard, sq_with_pawn - 1)) return 1;
        else return 0;
    }
}

U64 polykey_from_board() {
    int sq = 0, rank = 0, file = 0;
    U64 final_key = 0ULL;
    U64 bitboard;
    int piece;
    int poly_piece = 0;
    int offset = 0;

    // piece keys
    for (int piece = P; piece <= k; piece++) {
        bitboard = bitboards[piece];
        while (bitboard) {
            int square = get_lsb_index(bitboard);
            rank = 7 - floor(square/8);
            file = square % 8;
            poly_piece = polyglot_pieces[piece];

            final_key ^= polyglot_keys[(64 * poly_piece) + (8 * rank) + file];

            pop_bit(bitboard, square);
        }
    }

    // castling keys
    offset = 768;
    if (castle & wk) final_key ^= polyglot_keys[offset + 0];
    if (castle & wq) final_key ^= polyglot_keys[offset + 1];
    if (castle & bk) final_key ^= polyglot_keys[offset + 2];
    if (castle & bq) final_key ^= polyglot_keys[offset + 3];

    // enpassant keys
    offset = 772;
    if (has_enpassant_capture()) {
        file = enpassant % 8;
        final_key ^= polyglot_keys[offset + file];
    }

    // side keys
    if (side == white) final_key ^= polyglot_keys[780];

    return final_key;
}

unsigned short endian_swap_u16(unsigned short x) {
    x = (x>>8) | (x<<8);
    return x;
}

unsigned int endian_swap_u32(unsigned int x) {
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
    return x;
}

U64 endian_swap_u64(U64 x) {
    x = (x>>56) |
        ((x<<40) & 0x00FF000000000000) |
        ((x<<24) & 0x0000FF0000000000) |
        ((x<< 8) & 0x000000FF00000000) |
        ((x>> 8) & 0x00000000FF000000) |
        ((x>>24) & 0x0000000000FF0000) |
        ((x>>40) & 0x000000000000FF00) |
        (x<<56);
    return x;
}

void list_book_moves(U64 polykey) {
    int start = 0;
    s_polyglot_book_entry *entry;
    unsigned short move;
    for (entry = entries; entry<entries+num_entries; entry++) {
        if (polykey == endian_swap_u64(entry->key)) {
            move = endian_swap_u16(entry->move);
            printf("Key: %llx Index: %d Move: %c%d%c%d\n", endian_swap_u64(entry->key), start, 
                    files[((move >> 6) & 7)],
                    ((move >> 9) & 7) + 1,
                    files[((move >> 0) & 7)],
                    ((move >> 3) & 7) + 1);
        }
        start++;
    }
}

void get_book_move() {
    print_board();
    U64 polykey = polykey_from_board();
    // printf("Polykey: %llx\n",polykey);
    list_book_moves(polykey);
}