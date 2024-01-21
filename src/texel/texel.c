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
#define PATIENCE 2

void add_param(s_texel_params* params, char* name, int* param, int* n) {
    params[*n].name = name;
    params[*n].param = param;
    *n += 1;
}

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

void save_params(s_texel_params* params, int num_params, int epoch) {
    char path[100];

    snprintf(path, sizeof(path), "src/texel/session_data/epoch_%d.txt", epoch);
    printf("Writing to %s\n", path);    
    FILE *f = fopen(path, "w");

    int c = 1;
    fprintf(f, "int positional_score[2][6][64] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {\n");
        for (int j = 0; j < 6; j++) {
            fprintf(f, "        {");
            for (int k = 0; k < 64; k++) {
                fprintf(f, "%d, ", positional_score[i][j][k]);
                if (c++ % 8 == 0) {fprintf(f, "\n            ");}
            }
            fprintf(f, "},\n");
        }
        fprintf(f, "    },\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int doubled_pawn_penalty[2][8] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < 8; j++) {
            fprintf(f, "%d, ", doubled_pawn_penalty[i][j]);
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int isolated_pawn_penalty[2][8] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < 8; j++) {
            fprintf(f, "%d, ", isolated_pawn_penalty[i][j]);
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int backward_pawn_penalty[2][8] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < 8; j++) {
            fprintf(f, "%d, ", backward_pawn_penalty[i][j]);
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int pawn_defects[2][8] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < 8; j++) {
            fprintf(f, "%d, ", pawn_defects[i][j]);
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int unsupported_pawn_penalty[2] = {%d, %d};\n\n", unsupported_pawn_penalty[0], unsupported_pawn_penalty[1]);

    c = 1;
    fprintf(f, "int connected_pawn_bonus[2][64] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {");
        for (int j = 0; j < 64; j++) {
            fprintf(f, "%d, ", connected_pawn_bonus[i][j]);
            if (c++ % 8 == 0) {fprintf(f, "\n        ");}
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int passed_pawn[8] = {");
    for (int i = 0; i < 8; i++) {
        fprintf(f, "%d, ", passed_pawn[i]);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int passed_pawn_base[2] = {%d, %d};\n\n", passed_pawn_base[0], passed_pawn_base[1]);
    fprintf(f, "int passed_pawn_free_advance = %d;\n\n", passed_pawn_free_advance);
    fprintf(f, "int passed_pawn_partial_advance = %d;\n\n", passed_pawn_partial_advance);
    fprintf(f, "int passed_pawn_defended = %d;\n\n", passed_pawn_defended);
    fprintf(f, "int passed_pawn_partial_defended = %d;\n\n", passed_pawn_partial_defended);

    c = 1;
    fprintf(f, "int knight_outpost[2][64] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {   ");
        for (int j = 0; j < 64; j++) {
            fprintf(f, "%d, ", knight_outpost[i][j]);
            if (c++ % 8 == 0) {fprintf(f, "\n        ");}
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    c = 1;
    fprintf(f, "int bishop_outpost[2][64] = {\n");
    for (int i = 0; i < 2; i++) {
        fprintf(f, "    {   ");
        for (int j = 0; j < 64; j++) {
            fprintf(f, "%d, ", bishop_outpost[i][j]);
            if (c++ % 8 == 0) {fprintf(f, "\n        ");}
        }
        fprintf(f, "},\n");
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int bishop_pawns_on_color[2] = {%d, %d};\n\n", bishop_pawns_on_color[0], bishop_pawns_on_color[1]);
    fprintf(f, "int rook_semi_open_file_score = %d;\n\n", rook_semi_open_file_score);
    fprintf(f, "int rook_open_file_score[2] = {%d, %d};\n\n", rook_open_file_score[0], rook_open_file_score[1]);

    fprintf(f, "int king_open_file_penalty[8] = {");
    for (int i = 0; i < 8; i++) {
        fprintf(f, "%d, ", king_open_file_penalty[i]);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int king_semi_open_file_penalty[8] = {");
    for (int i = 0; i < 8; i++) {
        fprintf(f, "%d, ", king_semi_open_file_penalty[i]);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "int king_shield_bonus = %d;\n\n", king_shield_bonus);
    fprintf(f, "int bishop_mobility[2] = {%d, %d};\n\n", bishop_mobility[0], bishop_mobility[1]);
    fprintf(f, "int queen_mobility[2] = {%d, %d};\n\n", queen_mobility[0], queen_mobility[1]);
    fprintf(f, "int bishop_unit = %d;\n\n", bishop_unit);
    fprintf(f, "int queen_unit = %d;\n\n", queen_unit);

    fclose(f);
}

double get_best_k(s_texel* positions, int counter) {
    double start = 0.0, end = 10, step = 1.0;
    double curr = start, error;
    double best = get_error(positions, counter, start);
    for (int i = 0; i < 10; i++) {
        curr = start - step;
        while (curr < end) {
            curr += step;
            error = get_error(positions, counter, curr);
            if (error < best) {
                best = error;
                start = curr;
            }
        }
        end = start + step;
        start -= step;
        step /= 10;
    }
    return start;
}

void tune(s_texel* positions, int num_positions, s_board* position, s_info* info) {
    
    int n = 0;
    s_texel_params params[1260];

    /* Adding parameters */{
    char name[260];

    // piece square tables
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 64; k++) {
                sprintf(name, "positional_score[%d][%d][%d]", i, j ,k);
                add_param(params, name, &positional_score[i][j][k], &n);
            }
        }
    }

    // doubled pawn penalty 
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            sprintf(name, "doubled_pawn_penalty[%d][%d]", i, j);
            add_param(params, name, &doubled_pawn_penalty[i][j], &n);
        }
    }

    // isolated pawn penalty
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            sprintf(name, "isolated_pawn_penalty[%d][%d]", i, j);
            add_param(params, name, &isolated_pawn_penalty[i][j], &n);
        }
    }

    // backward pawn penalty
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            sprintf(name, "backward_pawn_penalty[%d][%d]", i, j);
            add_param(params, name, &backward_pawn_penalty[i][j], &n);
        }
    }

    // pawn defects
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            sprintf(name, "pawn_defects[%d][%d]", i, j);
            add_param(params, name, &pawn_defects[i][j], &n);
        }
    }

    // unsupported pawn penalty
    add_param(params, "unsupported_pawn_penalty[0]", &unsupported_pawn_penalty[0], &n);
    add_param(params, "unsupported_pawn_penalty[1]", &unsupported_pawn_penalty[1], &n);

    // Connected Pawn Bonus
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            sprintf(name, "connected_pawn_bonus[%d][%d]", i, j);
            add_param(params, name, &connected_pawn_bonus[i][j], &n);
        }
    }

    // Passed Pawn 
    for (int i = 0; i < 8; i++) {
        sprintf(name, "passed_pawn[%d]", i);
        add_param(params, name, &passed_pawn[i], &n);
    }
    
    add_param(params, "passed_pawn_base[0]", &passed_pawn_base[0], &n);
    add_param(params, "passed_pawn_base[1]", &passed_pawn_base[1], &n);
    add_param(params, "passed_pawn_free_advance", &passed_pawn_free_advance, &n);
    add_param(params, "passed_pawn_partial_advance", &passed_pawn_partial_advance, &n);
    add_param(params, "passed_pawn_defended", &passed_pawn_defended, &n);
    add_param(params, "passed_pawn_partial_defended", &passed_pawn_partial_defended, &n);
    
    // Knight and Bishop Outpost
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            sprintf(name, "knight_outpost[%d][%d]", i, j);
            add_param(params, name, &knight_outpost[i][j], &n);
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            sprintf(name, "bishop_outpost[%d][%d]", i, j);
            add_param(params, name, &bishop_outpost[i][j], &n);
        }
    }

    // Bishop Pawns on Color
    add_param(params, "bishop_pawns_on_color[0]", &bishop_pawns_on_color[0], &n);
    add_param(params, "bishop_pawns_on_color[1]", &bishop_pawns_on_color[1], &n);

    // Rook Semi Open and Open File Bonus
    add_param(params, "rook_semi_open_file_score", &rook_semi_open_file_score, &n);
    add_param(params, "rook_open_file_score[0]", &rook_open_file_score[0], &n);
    add_param(params, "rook_open_file_score[1]", &rook_open_file_score[1], &n);

    // King Open File Penalty
    for (int i = 0; i < 8; i++) {
        sprintf(name, "king_open_file_penalty[%d]", i);
        add_param(params, name, &king_open_file_penalty[i], &n);
    }

    // King Semi Open File Penalty
    for (int i = 0; i < 8; i++) {
        sprintf(name, "king_semi_open_file_penalty[%d]", i);
        add_param(params, name, &king_semi_open_file_penalty[i], &n);
    }

    // King Shield Bonus
    add_param(params, "king_shield_bonus", &king_shield_bonus, &n);

    // Bishop Mobility
    add_param(params, "bishop_mobility[0]", &bishop_mobility[0], &n);
    add_param(params, "bishop_mobility[1]", &bishop_mobility[1], &n);
    add_param(params, "bishop_unit", &bishop_unit, &n);

    // Queen Mobility
    add_param(params, "queen_mobility[0]", &queen_mobility[0], &n);
    add_param(params, "queen_mobility[1]", &queen_mobility[1], &n);
    add_param(params, "queen_unit", &queen_unit, &n);

    printf("Tuning %d parameters\n", n);
    }

    // Saving initial parameters
    save_params(params, n, 0);

    // Finding best value of K
    double K = get_best_k(positions, num_positions);
    printf("Using K: %f\n", K);

    // Finding initial error
    double best_error = get_error(positions, num_positions, K);
    printf("Initial error: %f\n", best_error);
    
    double base = 0, current_error, update_error;
    double gradient, error_param, rate, delta;

    // Initializing Adam parameters
    const double a = 0.3;
    const double b1 = 0.9;
    const double b2 = 0.999;
    const double epsilon = 1.0e-8;
    double *gradients = (double *) malloc(n * sizeof(double));
    double *momentum_1 = (double *) malloc(n * sizeof(double));
    double *momentum_2 = (double *) malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        momentum_1[i] = 0.0;
        momentum_2[i] = 0.0;
    }

    // Timers
    int gradient_start_time, gradient_end_time, update_start_time, update_end_time;
    int gradient_time = 0, update_time = 0;

    // Main tuning loop
    for (int epoch = 1; epoch <= 10000; epoch++) {

        // Calculating base error
        base = get_error(positions, num_positions, K);

        // Calculating gradients
        gradient_start_time = get_time_ms();
        for (int i = 0; i < n; i++) {
            *params[i].param += 1;
            error_param = get_error(positions, num_positions, K);
            gradients[i] = error_param - base;
            *params[i].param -= 1;
        }
        gradient_end_time = get_time_ms();
        gradient_time = gradient_end_time - gradient_start_time;

        printf("    ----Computed gradients for epoch %d in %.2f minutes\n", epoch, (double) gradient_time / 60000);

        // Updating parameters
        update_start_time = get_time_ms();
        for (int i = 0; i < n; i++) {
            gradient = gradients[i];
            momentum_1[i] = (b1 * momentum_1[i] + (1.0 - b1) * gradient) / (1 - pow(b1, epoch));
            momentum_2[i] = (b2 * momentum_2[i] + (1.0 - b2) * gradient * gradient) / (1 - pow(b2, epoch));
            rate = a * (1.0 - pow(b2, epoch)) / (1.0 - pow(b1, epoch));
            delta = rate * momentum_1[i] / (sqrt(momentum_2[i]) + epsilon);
            *params[i].param -= delta;
        }
        update_end_time = get_time_ms();
        update_time = update_end_time - update_start_time;

        printf("    ----Updated parameters for epoch %d in %d ms\n", epoch, update_time);

        current_error = get_error(positions, num_positions, K);
        printf("    Epoch: %d ==> Error: %f\n", epoch, current_error);

        save_params(params, n, epoch);

        if (epoch % PATIENCE == 0) {
            if (current_error < best_error) {
                best_error = current_error;
            } else {
                break;
            }
        }
    }

    free(momentum_1);
    free(momentum_2);
    free(gradients);

    printf("Final error: %f\n", best_error);
}