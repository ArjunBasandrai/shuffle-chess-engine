#pragma once

#include <string.h>

#include "board_constants.h"
#include "bit_manipulation.h"
#include "moves_list.h"
#include "pre_calculated_tables.h"
#include "zobrist.h"
#include "board.h"

// U64 bitboards_copy[12],occupancies_copy[3];
// int side_copy, enpaassant_copy,castle_copy, fifty_copy;
// U64 hash_key_copy;

struct copy_pos {
    U64 bitboards_copy[12];
    U64 occupancies_copy[3];
    int side_copy;
    int enpassant_copy;
    int castle_copy;
    int fifty_copy;
    U64 hash_key_copy;
};

extern void copy_board(s_board *pos, struct copy_pos *cc);
extern void take_back(s_board *pos, struct copy_pos *cc);

static inline int is_square_attacked(int square, int side, s_board *pos) {

    if ((side == white) && (pawn_attacks[black][square] & pos->bitboards[P])) return 1;

    if ((side == black) && (pawn_attacks[white][square] & pos->bitboards[p])) return 1;

    if (knight_attacks[square] & ((side == white) ? pos->bitboards[N] : pos->bitboards[n])) return 1;

    if (get_bishop_attacks(square,pos->occupancies[both]) & ((side == white) ? pos->bitboards[B] : pos->bitboards[b])) return 1;

    if (get_rook_attacks(square,pos->occupancies[both]) & ((side == white) ? pos->bitboards[R] : pos->bitboards[r])) return 1;

    if (get_queen_attacks(square,pos->occupancies[both]) & ((side == white) ? pos->bitboards[Q] : pos->bitboards[q])) return 1;

    if (king_attacks[square] & ((side == white) ? pos->bitboards[K] : pos->bitboards[k])) return 1;

    return 0;
}

void print_attacked_squares(int side, s_board *pos);

static inline void generate_moves(moves *move_list, s_board *pos) {
    move_list->count = 0;
    int source_square, target_square;

    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++) {
        bitboard = pos->bitboards[piece];

        //pawnn moves and castling

        if (pos->side == white) {

        // white pawn moves

            if (piece == P) {
                while (bitboard)
                {
                    source_square = get_lsb_index(bitboard);
                    target_square = source_square - 8;

                    // quiet pawn moves for white

                    if (!(target_square < a8) && !(get_bit(pos->occupancies[both],target_square))) {
                        if (source_square >= a7 && source_square <= h7) {
                            add_move(move_list, encode_move(source_square,target_square, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, N, 0, 0, 0, 0));

                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 0, 0, 0, 0));

                            if ((source_square >= a2 && source_square <= h2) && !(get_bit(pos->occupancies[both],target_square - 8))) {
                                add_move(move_list, encode_move(source_square,target_square-8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }

                    // capture moves for white pawns

                    attacks = pawn_attacks[pos->side][source_square] & pos->occupancies[black];

                    while (attacks) {
                        target_square = get_lsb_index(attacks);
                        if (source_square >= a7 && source_square <= h7) {
                            add_move(move_list, encode_move(source_square, target_square, piece, Q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, R, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, B, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square, target_square, piece, N, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks, target_square);
                    }

                    // enpassant captures for white

                    if (pos->enpassant != no_sq) {
                        U64 enpassant_attacks = pawn_attacks[pos->side][source_square] & (1ULL << pos->enpassant);

                        if (enpassant_attacks) {
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
            }

            if (piece == K) {
                if (pos->castle & wk) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(pos->occupancies[both], f1) && !get_bit(pos->occupancies[both], g1)) {
                        // checking if king square and f1 are not attacked
                        if (!is_square_attacked(e1,black,pos) && !is_square_attacked(f1,black,pos)) {
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }

                if (pos->castle & wq) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(pos->occupancies[both], d1) && !get_bit(pos->occupancies[both], c1) && !get_bit(pos->occupancies[both], b1)) {
                        // checking if king square and d1 are not attacked
                        if (!is_square_attacked(e1,black,pos) && !is_square_attacked(d1,black,pos)) {
                            add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }

        } 
        
        else {

        // black pawn moves

            if (piece == p) {
                while (bitboard)
                {
                    source_square = get_lsb_index(bitboard);
                    target_square = source_square + 8;

                    // quiet pawn moves for black

                    if (!(target_square > h1) && !(get_bit(pos->occupancies[both],target_square))) {
                        if (source_square >= a2 && source_square <= h2) {
                            add_move(move_list, encode_move(source_square,target_square, piece, q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, r, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, b, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, n, 0, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 0, 0, 0, 0));

                            if ((source_square >= a7 && source_square <= h7) && !(get_bit(pos->occupancies[both],target_square + 8))) {
                                add_move(move_list, encode_move(source_square,target_square + 8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }

                    // capture moves for black pawns

                    attacks = pawn_attacks[pos->side][source_square] & pos->occupancies[white];

                    while (attacks) {
                        target_square = get_lsb_index(attacks);
                        if (source_square >= a2 && source_square <= h2) {
                            add_move(move_list, encode_move(source_square,target_square, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, n, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks,target_square);
                    }

                    // enpassant captures for black

                    if (pos->enpassant != no_sq) {
                        U64 enpassant_attacks = pawn_attacks[pos->side][source_square] & (1ULL << pos->enpassant);

                        if (enpassant_attacks) {
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
                
            }
        
            if (piece == k) {
                if (pos->castle & bk) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(pos->occupancies[both], f8) && !get_bit(pos->occupancies[both], g8)) {
                        // checking if king square and f8 are not attacked
                        if (!is_square_attacked(e8,white,pos) && !is_square_attacked(f8,white,pos)) {
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }

                if (pos->castle & bq) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(pos->occupancies[both], d8) && !get_bit(pos->occupancies[both], c8) && !get_bit(pos->occupancies[both], b8)) {
                        // checking if king square and d8 are not attacked
                        if (!is_square_attacked(e8,white,pos) && !is_square_attacked(d8,white,pos)) {
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }

        }
    
        //knight moves

        if ((pos->side == white) ? piece == N: piece == n) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = knight_attacks[source_square] & ((pos->side == white)? ~pos->occupancies[white]: ~pos->occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((pos->side==white) ? pos->occupancies[black] : pos->occupancies[white],target_square)) {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    }

                    //capture move
                    else {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    pop_bit(attacks,target_square);
                }

                pop_bit(bitboard,source_square);
            }
        }

        // bishop moves

        if ((pos->side == white) ? piece == B: piece == b) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = get_bishop_attacks(source_square, pos->occupancies[both]) & ((pos->side == white)? ~pos->occupancies[white]: ~pos->occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((pos->side==white)? pos->occupancies[black] : pos->occupancies[white],target_square)) {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    }

                    //capture move
                    else {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    pop_bit(attacks,target_square);
                }

                pop_bit(bitboard,source_square);
            }
        }

        // rook moves

        if ((pos->side == white) ? piece == R: piece == r) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);

                attacks = get_rook_attacks(source_square, pos->occupancies[both]) & ((pos->side == white)? ~pos->occupancies[white]: ~pos->occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((pos->side==white)? pos->occupancies[black] : pos->occupancies[white],target_square)) {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    }

                    //capture move
                    else {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    pop_bit(attacks,target_square);
                }

                pop_bit(bitboard,source_square);
            }
        }

        // queen moves

        if ((pos->side == white) ? piece == Q : piece == q) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = get_queen_attacks(source_square, pos->occupancies[both]) & ((pos->side == white)? ~pos->occupancies[white]: ~pos->occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((pos->side==white)? pos->occupancies[black] : pos->occupancies[white],target_square)) {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    }

                    //capture move
                    else {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    pop_bit(attacks,target_square);
                }

                pop_bit(bitboard,source_square);
            }
        }
    
        //king moves

        if ((pos->side == white) ? piece == K: piece == k) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = king_attacks[source_square] & ((pos->side == white)? ~pos->occupancies[white]: ~pos->occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((pos->side==white)? pos->occupancies[black] : pos->occupancies[white],target_square)) {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
                    }

                    //capture move
                    else {
                        add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
                    }

                    pop_bit(attacks,target_square);
                }

                pop_bit(bitboard,source_square);
            }
        }

    }
}

static inline int make_move(int move, int move_flag, s_board *pos) {

    // quiet move

    if (move_flag == all_moves) {
        struct copy_pos movecopy;
        copy_board(pos,&movecopy);
        
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int double_push = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        pop_bit(pos->bitboards[piece], source_square);
        set_bit(pos->bitboards[piece], target_square);

        pos->hash_key ^= piece_keys[piece][source_square];
        pos->hash_key ^= piece_keys[piece][target_square];

        fifty++;

        if (piece == P || piece == p) {
            fifty = 0;
        }

        // handling captures

        if (capture) {

            fifty = 0;

            int start_piece, end_piece;
            if (pos->side == white) {
                start_piece = p;
                end_piece = k;
            } else {
                start_piece = P;
                end_piece = K;
            }

            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
                if (get_bit(pos->bitboards[bb_piece], target_square)) {
                    pop_bit(pos->bitboards[bb_piece], target_square);
                    pos->hash_key ^= piece_keys[bb_piece][target_square];
                    break;
                }
            }
        }

        // handling pawn promotions

        if (promoted) {
            if (pos->side == white) {
                pop_bit(pos->bitboards[P], target_square);
                pos->hash_key ^= piece_keys[P][target_square];
            } else {
                pop_bit(pos->bitboards[p], target_square);
                pos->hash_key ^= piece_keys[p][target_square];
            }
            set_bit(pos->bitboards[promoted], target_square);
            pos->hash_key ^= piece_keys[promoted][target_square];
        }

        // handling enpassant moves

        if (enpass) {
            if (pos->side == white) {
                pop_bit(pos->bitboards[p],target_square + 8);
                pos->hash_key ^= piece_keys[p][target_square + 8];
            } else {
                pop_bit(pos->bitboards[P],target_square - 8);
                pos->hash_key ^= piece_keys[P][target_square - 8];
            }
        }

        /************IMPORTANT************/
        /****reseting enpassant square****/

        if (pos->enpassant != no_sq) {
            pos->hash_key ^= enpassant_keys[pos->enpassant];
        }

        pos->enpassant = no_sq;


        // handling double pawn pushes

        if (double_push) {
            if (pos->side == white) {
                pos->enpassant = target_square + 8;
                pos->hash_key ^= enpassant_keys[target_square + 8];
            } else {
                pos->enpassant = target_square - 8;
                pos->hash_key ^= enpassant_keys[target_square - 8];
            }
        }

        // handling castling mves

        if (castling) {
            switch (target_square) {
                case (g1):
                    pop_bit(pos->bitboards[R],h1);
                    set_bit(pos->bitboards[R],f1);
                    pos->hash_key ^= piece_keys[R][h1];
                    pos->hash_key ^= piece_keys[R][f1];
                    break;
                case (c1):
                    pop_bit(pos->bitboards[R],a1);
                    set_bit(pos->bitboards[R],d1);
                    pos->hash_key ^= piece_keys[R][a1];
                    pos->hash_key ^= piece_keys[R][d1];
                    break;
                case (g8):
                    pop_bit(pos->bitboards[r],h8);
                    set_bit(pos->bitboards[r],f8);
                    pos->hash_key ^= piece_keys[r][h8];
                    pos->hash_key ^= piece_keys[r][f8];
                    break;
                case (c8):
                    pop_bit(pos->bitboards[r],a8);
                    set_bit(pos->bitboards[r],d8);
                    pos->hash_key ^= piece_keys[r][a8];
                    pos->hash_key ^= piece_keys[r][d8];
                    break;
            }
        }

        // updating castling rights

        pos->hash_key ^= castle_keys[pos->castle];

        pos->castle &= castling_rights[source_square];
        pos->castle &= castling_rights[target_square];

        pos->hash_key ^= castle_keys[pos->castle];

        // updating occupancies

        memset(pos->occupancies, 0ULL, 24);

        for (int bb_piece = P; bb_piece <= K; bb_piece++) {
            pos->occupancies[white] |= pos->bitboards[bb_piece];
        }

        for (int bb_piece = p; bb_piece <= k; bb_piece++) {
            pos->occupancies[black] |= pos->bitboards[bb_piece];
        }

        pos->occupancies[both] |= pos->occupancies[white];
        pos->occupancies[both] |= pos->occupancies[black];

        // checking if square is in check after move
        pos->side ^= 1;

        pos->hash_key ^= side_key;

        // U64 hash_from_scratch = generate_hash_keys();

        // if (hash_key != hash_from_scratch) {
        //     printf("\nMake move\nmove: ");
        //     print_move(move);
        //     printf("\n");
        //     print_board();
        //     printf("hash key should be: %llx\n",hash_from_scratch);
        //     getchar();
        // }

        if (is_square_attacked(get_lsb_index((pos->side == white) ? pos->bitboards[k] : pos->bitboards[K]), pos->side, pos)) {
            take_back(pos,&movecopy);
            return 0;
        }
        else {
            return 1;
        }
    }

    // capture move

    else {

        if (get_move_capture(move)) {
            make_move(move,all_moves, pos);
        }

        else {
            return 0;
        }

    }
}