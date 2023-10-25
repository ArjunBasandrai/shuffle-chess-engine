#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

#define infinity 50000
#define mate_value 49000
#define mate_score 48000
#define draw_score 0

enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

enum {
    white,
    black,
    both
};

enum {
    rook,
    bishop
};

enum {
    wk = 1, wq = 2,
    bk = 4, bq = 8
};

enum {
    P, N, B, R, Q, K,
    p, n, b, r, q, k
};

enum {
    all_moves, only_captures
};

extern char ascii_pieces[12];
extern char *unicode_pieces[12];
extern int char_pieces[];

extern char files[8];

extern const char *sqaure_to_coordinate[];

extern const U64 not_a_file;
extern const U64 not_h_file;
extern const U64 not_hg_file;
extern const U64 not_ab_file;

extern const int bishop_relevant_bits[64];

extern const int rook_relevant_bits[64];

extern const int castling_rights[64];

extern const int material_score[2][12];

extern const int opening_phase_score;
extern const int endgame_phase_score;

enum { opening, endgame, middlegame };

enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

extern const int positional_score[2][6][64];
extern const int mirror_score[128];

static int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

extern const int full_depth_moves;
extern const int reduction_limit;

extern U64 hash_key;
extern int ply;

extern U64 repetitions_table[1000];
extern int repetition_index;

extern U64 file_mask[64];
extern U64 rank_mask[64];
extern U64 isolated_mask[64];
extern U64 white_passed_mask[64];
extern U64 black_passed_mask[64];

extern int get_rank[64];

extern char engine_name[];

typedef struct {
    int use_book;
} s_options;

extern s_options engine_options[1];