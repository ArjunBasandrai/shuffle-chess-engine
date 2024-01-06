#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#ifndef WINDOWS
#define WINDOWS
#include <windows.h>
#endif
#else
#ifndef UNIX
#define UNIX
#include <unistd.h>
#endif
#endif

#include "../bit_manipulation.h"
#include "../board_constants.h"
#include "../moves_list.h"
#include "../pre_calculated_tables.h"
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

const int poly_promotions[] = {
    n, b, r, q
};

void init_poly_book() {
    srand(time(NULL));
    engine_options->use_book = 0;

    char exe_path[260];
    #ifdef WINDOWS
    GetModuleFileNameA(NULL, exe_path, 260);
    #else
    readlink("/proc/self/exe", exe_path, 260);
    #endif
    char *dir_path = NULL;
    dir_path = strrchr(exe_path, '/');
    if (dir_path == NULL) {
        dir_path = strrchr(exe_path, '\\');  // Check for Windows path separator
    }

    if (dir_path != NULL) {
        *(dir_path + 1) = '\0';
    } else {
        perror("Error: No directory separator found in path");
    }

    size_t length = dir_path - exe_path;
    char *path = (char*)malloc(length + 1);
    strncpy(path, exe_path, length);
    path[length] = '\0';

    char relative_path[260];
    strcpy(relative_path, path);
    strcat(relative_path, "\\shuffle.bin");

    FILE *pFile = fopen(relative_path, "rb");
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

        if (num_entries > 0) engine_options->use_book = 1;
    }
}

void clean_poly_book() {
    free(entries);
}

int has_enpassant_capture(s_board *pos) {
    int sq_with_pawn = 0;
    int target_piece = (pos->side == white) ? P : p;
    if (pos->enpassant != no_sq) {
        if (pos->side == white) {
            sq_with_pawn = pos->enpassant + 8;
        } else {
            sq_with_pawn = pos->enpassant - 8;
        }

        U64 bitboard = pos->bitboards[target_piece];

        if (get_bit(bitboard, sq_with_pawn + 1)) return 1;
        else if (get_bit(bitboard, sq_with_pawn - 1)) return 1;
        else return 0;
    }
}

U64 polykey_from_board(s_board *pos) {
    int sq = 0, rank = 0, file = 0;
    U64 final_key = 0ULL;
    U64 bitboard;
    int piece;
    int poly_piece = 0;
    int offset = 0;

    // piece keys
    for (int piece = P; piece <= k; piece++) {
        bitboard = pos->bitboards[piece];
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
    if (pos->castle & wk) final_key ^= polyglot_keys[offset + 0];
    if (pos->castle & wq) final_key ^= polyglot_keys[offset + 1];
    if (pos->castle & bk) final_key ^= polyglot_keys[offset + 2];
    if (pos->castle & bq) final_key ^= polyglot_keys[offset + 3];

    // enpassant keys
    offset = 772;
    if (has_enpassant_capture(pos)) {
        file = pos->enpassant % 8;
        final_key ^= polyglot_keys[offset + file];
    }

    // side keys
    if (pos->side == white) final_key ^= polyglot_keys[780];

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

int polymove_to_inmove(unsigned short move, s_board *pos) {
    int ff = ((move >> 6) & 7);
    int fr = 8 - ((move >> 9) & 7);
    int tf = ((move >> 0) & 7);
    int tr = 8 - ((move >> 3) & 7);
    int pp = ((move >> 12) & 7);

    int source = (fr-1) * 8 + ff;
    int target = (tr-1) * 8 + tf;

    int p;
    if (pp && (pos->side == white)) {
        switch (pp) {
            case 1: p=N; break;
            case 2: p=B; break;
            case 3: p=R; break;
            case 4: p=Q; break;
        }
    } else if (pp && (pos->side == black)) {
        switch (pp) {
            case 1: p=n; break;
            case 2: p=b; break;
            case 3: p=r; break;
            case 4: p=q; break;
        }
    }

    U64 bitboard;
    int sp = -1;
    for (int piece = P; piece <= k; piece++) {
        bitboard = pos->bitboards[piece];
        while (bitboard) {
            int square = get_lsb_index(bitboard);
            if (square == source) {
                sp = piece;
                break;
            }
            pop_bit(bitboard, square);
        }
    }

    int capture;
    if (!get_bit(pos->occupancies[both], target)) {
        capture = 0;
    } else {
        capture = 1;
    }

    int db;
    if (((pos->side == white) && (source >= a2) && (source <= h2) && (source == target + 16)) || ((pos->side == black) && (source >= a7) && (source <= h7) && (source == target - 16))) {
        db = 1;
    } else {
        db = 0;
    }

    int enpass=0;
    if (pos->enpassant) {
        U64 enpassant_attacks = pawn_attacks[pos->side][source] & (1ULL << pos->enpassant);
        if (enpassant_attacks) {
            int target_enpassant = get_lsb_index(enpassant_attacks);
            if (target_enpassant == target) {enpass = 1;}
        }
    }

    int castle;
    if (((sp == K)||(sp == k)) && (target - source == -4)) {
        target += 2;
        castle = 1;
    }
    // kingside castling polyglot format has h1 instead of g1
     else if (((sp == K)||(sp == k)) && (target - source == 3)) {
        target--;
        castle = 1;
    } else {
        castle = 0;
    }

    return encode_move(source,target,sp,(pp!=0)?p:0,capture,db,enpass,castle);
}

int get_book_move(s_board *pos) {
    U64 polykey = polykey_from_board(pos);
    s_polyglot_book_entry *entry;
    unsigned short move;
    int temp_move;
    const int max_book_moves = 32;
    int book_moves[32];
    int count = 0;

    // move weights
    int weights[32];
    int temp_weight;
    int w_count = 0;
    int weight_sum = 0;

    for (entry = entries; entry<entries+num_entries; entry++) {
        if (polykey == endian_swap_u64(entry->key)) {
            move = endian_swap_u16(entry->move);
            int source_file = ((move >> 6) & 7);
            temp_move = polymove_to_inmove(move, pos);
            if (temp_move) {
                book_moves[count++] = temp_move;

                temp_weight = endian_swap_u16(entry->weight);
                weights[w_count++] = temp_weight;
                weight_sum += temp_weight;

                if (count > max_book_moves) break; 
            }            
        }
    }

    if (count) {
        int index;
        int random_weight = rand() % weight_sum;
        for (int i=0; i<count; i++) {
            random_weight = random_weight - weights[i];
            if (random_weight <= 0 ) {
                index = i;
                break;
            }
        }
        return book_moves[index];
    } else {
        return 0;
    }
}