#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "texel.h"
#include "../board_constants.h"
#include "../threading/tinycthread.h"

#define THREADS 10

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

int get_thread_error(void* arg) {
    s_texel_thread_data* data = (s_texel_thread_data*)arg;
    int score;
    double sigmoid_result;
    s_board position[1];
    s_info info[1];
    position->enpassant = no_sq;
    position->age = 0;
    info->quit = 0;
    info->threads = 1;
    // printf("Thread %d started...\n", data->thread_id);
    for (int i = data->start_idx; i < data->end_idx; i++) {
        parse_fen(data->positions[i].fen, position);
        score = quiescence(-infinity, infinity, position, info);
        sigmoid_result = 1 / (1 + pow(10, - data->K * score / 400.0));
        data->error[data->thread_id] += pow(data->positions[i].result - sigmoid_result, 2);
    }
    return 0;
}

double get_error(s_texel* positions, int num_positions, double K) {
    thrd_t threads[THREADS];
    int batch_size = num_positions / THREADS;
    num_positions -= num_positions % batch_size;
    double errors[THREADS];
    for (int i = 0; i < THREADS; i++) {
        errors[i] = 0.0;
    }
    int num_batches = num_positions / batch_size;
    for (int i = 0; i < THREADS; i++) {
        s_texel_thread_data* data = (s_texel_thread_data*)malloc(sizeof(s_texel_thread_data));
        data->error = errors;
        data->positions = positions;
        data->batch_size = batch_size;
        data->K = K;
        data->start_idx = i * batch_size;
        data->end_idx = (i + 1) * batch_size;
        data->thread_id = i;
        thrd_create(&threads[i], &get_thread_error, (void*)data);
    }

    double error = 0.0;

    for (int i = 0; i < THREADS; i++) {
        thrd_join(threads[i], NULL);
        error += errors[i];
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
    best_error = get_error(positions, num_positions, K);
    printf("Initial error: %f\n", best_error);
    while (improved) {
        improved = 0;
        counter++;
        for (int i = 0; i < 2; i++) {
            printf("    Pass %d => Tuning PSTs for %s...\n", counter, (i) ? "black" : "white");
            for (int j = 0; j< 6; j++) {
                for (int k = 0; k < 64; k++) {
                    pst[i][j][k] += adjust;
                    positional_score[i][j][k] += adjust;
                    new_error = get_error(positions, num_positions, K);
                    if (new_error < best_error) {
                        best_error = new_error;
                        improved = 1;
                    } else {
                        pst[i][j][k] -= 2 * adjust;
                        positional_score[i][j][k] -= 2 * adjust;
                        new_error = get_error(positions, num_positions, K);
                        if (new_error < best_error) {
                            best_error = new_error;
                            improved = 1;
                        } else {
                            pst[i][j][k] += adjust;
                            positional_score[i][j][k] += adjust;
                        }
                    }
                }
            }
        }

        printf("Best error after %dth pass: %f\n", counter, best_error);

        if (counter % 2 == 0) {
            for (int i = 0; i< 2; i++) {
                for (int j = 0; j< 6; j++) {
                    for (int k = 0; k< 64; k++) {
                        printf("%d ", pst[i][j][k]);
                    }
                    printf("\n");
                }
                printf("\n");
            }
        }
    }

    for (int i = 0; i< 2; i++) {
        for (int j = 0; j< 6; j++) {
            for (int k = 0; k< 64; k++) {
                printf("%d ", pst[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}