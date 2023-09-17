#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#ifndef CONST_H_
#define CONST_H_
#include "board_constants.h"
#endif

#ifndef PCTABLES_H_
#define PCTABLES_H_
#include "pre_calculated_tables.h"
#endif

#ifndef BOARD_H_
#define BOARD_H_
#include "board.h"
#endif

#ifndef MOVES_H_
#define MOVES_H_
#include "moves_list.h"
#endif

#define copy_board() \
    U64 bitboards_copy[12],occupancies_copy[3]; \
    int side_copy, enpaassant_copy,castle_copy; \
    memcpy(bitboards_copy, bitboards, sizeof(bitboards)); \
    memcpy(occupancies_copy, occupancies, sizeof(occupancies)); \
    side_copy = side, enpaassant_copy = enpassant, castle_copy = castle;

#define take_back() \
    memcpy(bitboards, bitboards_copy, sizeof(bitboards)); \
    memcpy(occupancies,occupancies_copy,sizeof(occupancies)); \
    side = side_copy, enpassant = enpaassant_copy, castle = castle_copy; 

static inline int is_square_attacked(int square, int side) {

    if ((side == white) && (pawn_attacks[black][square] & bitboards[P])) return 1;

    if ((side == black) && (pawn_attacks[white][square] & bitboards[p])) return 1;

    if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) return 1;

    if (get_bishop_attacks(square,occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;

    if (get_rook_attacks(square,occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;

    if (get_queen_attacks(square,occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;

    if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) return 1;

    return 0;
}

void print_attacked_squares(int side) {

    printf("\n");
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            if (!file) {
                printf("%d  ", 8 - rank);
            }
            printf("%d ",is_square_attacked(square, side) ? 1 : 0);
        }
        printf("\n");
    }

    printf("\n   a b c d e f g h\n\n");

}

static inline void generate_moves(moves *move_list) {
    move_list->count = 0;
    int source_square, target_square;

    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++) {
        bitboard = bitboards[piece];

        //pawnn moves and castling

        if (side == white) {

        // white pawn moves

            if (piece == P) {
                while (bitboard)
                {
                    source_square = get_lsb_index(bitboard);
                    target_square = source_square - 8;

                    // quiet pawn moves for white

                    if (!(target_square < a8) && !(get_bit(occupancies[both],target_square))) {
                        if (source_square >= a7 && source_square <= h7) {
                            add_move(move_list, encode_move(source_square,target_square, piece, Q, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, R, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, B, 0, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, N, 0, 0, 0, 0));

                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 0, 0, 0, 0));

                            if ((source_square >= a2 && source_square <= h2) && !(get_bit(occupancies[both],target_square - 8))) {
                                add_move(move_list, encode_move(source_square,target_square-8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }

                    // capture moves for white pawns

                    attacks = pawn_attacks[side][source_square] & occupancies[black];

                    while (attacks) {
                        target_square = get_lsb_index(attacks);
                        if (source_square >= a7 && source_square <= h7) {
                            add_move(move_list, encode_move(source_square,target_square, piece, Q, 1, 1, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, R, 1, 1, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, B, 1, 1, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, N, 1, 1, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 1, 0, 0, 0));
                        }
                        pop_bit(attacks,target_square);
                    }

                    // enpassant captures for white

                    if (enpassant != no_sq) {
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        if (enpassant_attacks) {
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_square,target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
            }

            if (piece == K) {
                if (castle & wk) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1)) {
                        // checking if king square and f1 are not attacked
                        if (!is_square_attacked(e1,black) && !is_square_attacked(f1,black)) {
                            add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }

                if (castle & wq) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(occupancies[both], d1) && !get_bit(occupancies[both], c1) && !get_bit(occupancies[both], b1)) {
                        // checking if king square and d1 are not attacked
                        if (!is_square_attacked(e1,black) && !is_square_attacked(d1,black)) {
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

                    if (!(target_square > h1) && !(get_bit(occupancies[both],target_square))) {
                        if (source_square >= a2 && source_square <= h2) {
                            add_move(move_list, encode_move(source_square,target_square, piece, q, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, r, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, b, 1, 0, 0, 0));
                            add_move(move_list, encode_move(source_square,target_square, piece, n, 1, 0, 0, 0));
                        } else {
                            add_move(move_list, encode_move(source_square,target_square, piece, 0, 0, 0, 0, 0));

                            if ((source_square >= a7 && source_square <= h7) && !(get_bit(occupancies[both],target_square + 8))) {
                                add_move(move_list, encode_move(source_square,target_square + 8, piece, 0, 0, 1, 0, 0));
                            }
                        }
                    }

                    // capture moves for black pawns

                    attacks = pawn_attacks[side][source_square] & occupancies[white];

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

                    if (enpassant != no_sq) {
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        if (enpassant_attacks) {
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            add_move(move_list, encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    pop_bit(bitboard,source_square);
                }
                
            }
        
            if (piece == k) {
                if (castle & bk) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8)) {
                        // checking if king square and f8 are not attacked
                        if (!is_square_attacked(e8,white) && !is_square_attacked(f8,white)) {
                            add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }

                if (castle & bq) {
                    // checking if squares in between are unoccupied
                    if (!get_bit(occupancies[both], d8) && !get_bit(occupancies[both], c8) && !get_bit(occupancies[both], b8)) {
                        // checking if king square and d8 are not attacked
                        if (!is_square_attacked(e8,white) && !is_square_attacked(d8,white)) {
                            add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
                        }
                    }
                }
            }

        }
    
        //knight moves

        if ((side == white) ? piece == N: piece == n) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = knight_attacks[source_square] & ((side == white)? ~occupancies[white]: ~occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((side==white) ? occupancies[black] : occupancies[white],target_square)) {
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

        if ((side == white) ? piece == B: piece == b) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ((side == white)? ~occupancies[white]: ~occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((side==white)? occupancies[black] : occupancies[white],target_square)) {
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

        if ((side == white) ? piece == R: piece == r) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);

                attacks = get_rook_attacks(source_square, occupancies[both]) & ((side == white)? ~occupancies[white]: ~occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((side==white)? occupancies[black] : occupancies[white],target_square)) {
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

        if ((side == white) ? piece == Q : piece == q) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = get_queen_attacks(source_square, occupancies[both]) & ((side == white)? ~occupancies[white]: ~occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((side==white)? occupancies[black] : occupancies[white],target_square)) {
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

        if ((side == white) ? piece == K: piece == k) {
            while (bitboard) {
                source_square = get_lsb_index(bitboard);
                
                attacks = king_attacks[source_square] & ((side == white)? ~occupancies[white]: ~occupancies[black]);

                while (attacks) {
                    target_square = get_lsb_index(attacks);
                    //quiet move
                    if (!get_bit((side==white)? occupancies[black] : occupancies[white],target_square)) {
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

static inline int make_move(int move, int move_flag) {

    // quiet move

    if (move_flag == all_moves) {

        copy_board();
        
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int double_push = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        pop_bit(bitboards[piece], source_square);
        set_bit(bitboards[piece], target_square);

        // handling captures

        if (capture) {

            int start_piece, end_piece;
            if (side == white) {
                start_piece = p;
                end_piece = k;
            } else {
                start_piece = P;
                end_piece = K;
            }

            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
                if (get_bit(bitboards[bb_piece], target_square)) {
                    pop_bit(bitboards[bb_piece], target_square);
                    break;
                }
            }
        }

        // handling pawn promotions

        if (promoted) {
            pop_bit(bitboards[(side == white) ? P : p], target_square);
            set_bit(bitboards[promoted], target_square);
        }

        // handling enpassant moves

        if (enpass) {
            (side == white) ? pop_bit(bitboards[p],target_square + 8) :
                              pop_bit(bitboards[P],target_square - 8);
        }

        /************IMPORTANT************/
        /****reseting enpassant square****/

        enpassant = no_sq;

        // handling double pawn pushes

        if (double_push) {
            enpassant = (side == white) ? target_square + 8 : target_square - 8;
        }

        // handling castling mves

        if (castling) {
            switch (target_square) {
                case (g1):
                    pop_bit(bitboards[R],h1);
                    set_bit(bitboards[R],f1);
                    break;
                case (c1):
                    pop_bit(bitboards[R],a1);
                    set_bit(bitboards[R],d1);
                    break;
                case (g8):
                    pop_bit(bitboards[r],h8);
                    set_bit(bitboards[r],f8);
                    break;
                case (c8):
                    pop_bit(bitboards[r],a8);
                    set_bit(bitboards[r],d8);
                    break;
            }
        }

        // updating castling rights

        castle &= castling_rights[source_square];
        castle &= castling_rights[target_square];

        // updating occupancies

        memset(occupancies, 0ULL, 24);

        for (int bb_piece = P; bb_piece <= K; bb_piece++) {
            occupancies[white] |= bitboards[bb_piece];
        }

        for (int bb_piece = p; bb_piece <= k; bb_piece++) {
            occupancies[black] |= bitboards[bb_piece];
        }

        occupancies[both] |= occupancies[white];
        occupancies[both] |= occupancies[black];

        // checking if square is in check after move

        side ^= 1;
        if (is_square_attacked(get_lsb_index((side == white) ? bitboards[k] : bitboards[K]), side)) {
            take_back();
            return 0;
        }
        else {
            return 1;
        }
    }

    // capture move

    else {

        if (get_move_capture(move)) {
            make_move(move,all_moves);
        }

        else {
            return 0;
        }

    }
}