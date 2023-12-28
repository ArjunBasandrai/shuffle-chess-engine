#pragma once

#include "board.h"
#include "board_constants.h"

#define encode_move(source,target,piece,promoted,capture,double,enpassant,castling) \
    (source) | (target << 6) | \
    (piece << 12) | (promoted << 16) | \
    (capture << 20) | (double << 21) | \
    (enpassant << 22) | (castling << 23)

typedef struct {
    int moves[256];
    int count;
} moves;

#define get_move_source(move) (move & 0x3f)
#define get_move_target(move) ((move & 0xfc0)) >> 6
#define get_move_piece(move) ((move & 0xf000)) >> 12
#define get_move_promoted(move) ((move & 0xf0000)) >> 16
#define get_move_capture(move) (move & 0x100000)
#define get_move_double(move) (move & 0x200000)
#define get_move_enpassant(move) (move & 0x400000)
#define get_move_castling(move) (move & 0x800000)

int get_captured_piece(int move, s_board *pos);

static inline void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

extern char promoted_pieces[];

extern void print_move(int move);
extern void print_move_list(moves *move_list);