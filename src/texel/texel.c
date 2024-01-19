#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "texel.h"
#include "../board_constants.h"
#include "../gettime.h"
#include "../evaluation.h"
#include "../threading/tinycthread.h"

#define THREADS 15
#define PATIENCE 3

s_texel* read_files(char *fen_path, char* score_path, int *counter) {
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
    s_board position[1];
    s_info info[1];
    position->enpassant = no_sq;
    position->age = 0;
    info->quit = 0;
    info->threads = 1;

    while (fgets(buffer, 1024, f) != NULL) {
        parse_fen(buffer, position);
        // if (evaluate(position) != quiescence(-infinity, infinity, position, info)) {
        //     continue;
        // }
        buffer[strcspn(buffer, "\n")] = '\0';
        positions->fen = (char*)malloc(1024 * sizeof(char));
        strcpy(positions->fen,buffer);
        fgets(buffer, 1024, s);
        buffer[strcspn(buffer, "\n")] = '\0';
        positions->result = atof(buffer);
        positions++;
        (*counter)++;
    }

    positions = positions_start;

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
        // score = quiescence(-infinity, infinity, position, info);
        score = evaluate(position);
        if (position->side == black) {
            score = -score;
        }
        sigmoid_result = (double)1 / (1 + pow(exp(1), - data->K * score));
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

void print_params() {
    int c = 1;
    for (int i = 0; i< 2; i++) {
        for (int j = 0; j< 6; j++) {
            for (int k = 0; k< 64; k++) {
                printf("%d, ", positional_score[i][j][k]);
                if (c++ % 8 == 0) {printf("\n");}
            }
            printf("\n");
        }
        printf("\n");
    }
}

void save_params(int epoch) {
    int c;
    char path[100];
    char epoch_buffer[10];

    sprintf(epoch_buffer, "%d", epoch);
    snprintf(path, sizeof(path), "src/texel/session_data/epoch_%s.txt", epoch_buffer);
    printf("Writing to %s\n", path);    
    FILE *f = fopen(path, "w");
    c = 1;
    for (int i = 0; i< 2; i++) {
        for (int j = 0; j< 6; j++) {
            for (int k = 0; k< 64; k++) {
                fprintf(f, "%d, ", positional_score[i][j][k]);
                if (c++ % 8 == 0) {fprintf(f, "\n");}
            }
            fprintf(f, "\n");
        }
        fprintf(f,"\n");
    }
    fclose(f);
}

void tune(s_texel* positions, double K, int num_positions, s_board* position, s_info* info) {

    double best_error = get_error(positions, num_positions, K);
    printf("Initial error: %f\n", best_error);
    
    double base = 0, current_error, update_error;
    double gradients[2][6][64];
    double gradient, error_param, rate, delta;

    const double a = 0.3;
    const double b1 = 0.9;
    const double b2 = 0.999;
    const double epsilon = 1.0e-8;
    double m[2][6][64];
    double v[2][6][64];

    for (int i = 0; i< 2; i++) {
        for (int j = 0; j< 6; j++) {
            for (int k = 0; k < 64; k++) {
                m[i][j][k] = 0.0;
                v[i][j][k] = 0.0;
            }
        }
    }

    int gradient_start_time, gradient_end_time, update_start_time, update_end_time;
    int gradient_time = 0, update_time = 0;

    for (int epoch = 1; epoch <= 10000; epoch++) {

        // Calculating base error
        base = get_error(positions, num_positions, K);

        // Calculating gradients
        gradient_start_time = get_time_ms();
        for (int i = 0; i< 2; i++) {
            for (int j = 0; j< 6; j++) {
                for (int k = 0; k < 64; k++) {
                    positional_score[i][j][k] += 1;
                    error_param = get_error(positions, num_positions, K);
                    gradients[i][j][k] = error_param - base;
                    positional_score[i][j][k] -= 1;
                }
            }
        }
        gradient_end_time = get_time_ms();
        gradient_time = gradient_end_time - gradient_start_time;

        printf("    ----Computed gradients for epoch %d in %.2f minutes\n", epoch, (double) gradient_time / 60000);

        // Updating parameters
        update_start_time = get_time_ms();
        for (int i = 0; i< 2; i++) {
            for (int j = 0; j< 6; j++) {
                for (int k = 0; k< 64; k++) {
                    gradient = gradients[i][j][k];
                    m[i][j][k] = (b1 * m[i][j][k] + (1.0 - b1) * gradient) / (1 - pow(b1, epoch));
                    v[i][j][k] = (b2 * v[i][j][k] + (1.0 - b2) * gradient * gradient) / (1 - pow(b2, epoch));
                    rate = a * (1.0 - pow(b2, epoch)) / (1.0 - pow(b1, epoch));
                    delta = rate * m[i][j][k] / (sqrt(v[i][j][k]) + epsilon);
                    positional_score[i][j][k] -= delta;
                }
            }
        }
        update_end_time = get_time_ms();
        update_time = update_end_time - update_start_time;

        printf("    ----Updated parameters for epoch %d in %d ms\n", epoch, update_time);

        current_error = get_error(positions, num_positions, K);
        printf("    Epoch: %d ==> Error: %f\n", epoch, current_error);

        save_params(epoch);

        if (epoch % PATIENCE == 0) {
            if (current_error < best_error) {
                best_error = current_error;
            } else {
                break;
            }
        }
    }

    print_params();
    printf("Final error: %f\n", best_error);

}