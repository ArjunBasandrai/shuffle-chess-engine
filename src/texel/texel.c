#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "texel.h"
#include "../board_constants.h"

s_texel* read_files(char *fen_path, char* score_path) {
    FILE *f = fopen(fen_path, "r");
    FILE *s = fopen(score_path, "r");

    if (f == NULL) {
        printf("Error opening file %s\n", fen_path);
        exit(1);
    }

    if (s == NULL) {
        printf("Error opening file %s\n", score_path);
        exit(1);
    }

    assert(f != NULL && s != NULL);

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    s_texel* positions = (s_texel*)malloc(fsize * sizeof(s_texel));
    s_texel* positions_start = positions;
    char* buffer = (char*)malloc(1024 * sizeof(char));
    while (fgets(buffer, 1024, f) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        positions->fen = (char*)malloc(1024 * sizeof(char));
        strcpy(positions->fen,buffer);
        fgets(buffer, 1024, s);
        buffer[strcspn(buffer, "\n")] = '\0';
        positions->result = atof(buffer);
        positions++;
    }

    positions  = positions_start;

    fclose(f);
    fclose(s);
    free(buffer);

    return positions;
}

double get_error(s_texel* positions, int num_positions, double K, s_board* position, s_info* info) {
    double error = 0.0;
    double sigmoid_result;
    int score;
    for (int i = 0; i < num_positions; i++) {
        parse_fen(positions[i].fen, position);
        score = quiescence(-infinity, infinity, position, info);
        sigmoid_result = 1 / (1 + pow(10, - K * score / 400.0));
        error += pow(positions[i].result - sigmoid_result, 2);
    }
    error /= num_positions;
    return error;
}

void tune(s_texel* positions, double K, int num_positions, s_board* position, s_info* info) {
    int pst[2][6][64];
    for (int i = 0; i< 2; i++) {
        for (int j = 0; j< 6; j++) {
            for (int k = 0; k< 64; k++) {
                pst[i][j][k] = positional_score[i][j][k];
            }
        }
    }
    int adjust = 1;
    int improved = 1;
    double new_error, best_error;
    int counter = 0;
    // 754006
    best_error = get_error(positions, num_positions, K, position, info);
    while (improved) {
        improved = 0;
        for (int i=0; i<64; i++) {
            pst[0][0][i] += adjust;
            positional_score[0][0][i] += adjust;
            new_error = get_error(positions, num_positions, K, position, info);
            if (new_error < best_error) {
                best_error = new_error;
                improved = 1;
            } else {
                pst[0][0][i] -= 2 * adjust;
                positional_score[0][0][i] -= 2 * adjust;
                new_error = get_error(positions, num_positions, K, position, info);
                if (new_error < best_error) {
                    best_error = new_error;
                    improved = 1;
                } else {
                    pst[0][0][i] += adjust;
                    positional_score[0][0][i] += adjust;
                }
            }
        }
        printf("Best error after %dth pass: %f\n", ++counter, best_error);

        if (counter % 20 == 0) {
            for (int i=0; i<8; i++) {
                for (int j=0; j<8; j++) {
                    printf("%d ", pst[0][0][i * 8 + j]);
                }
                printf("\n");
            }
        }
    }

    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            printf("%d ", pst[0][0][i * 8 + j]);
        }
        printf("\n");
    }
}