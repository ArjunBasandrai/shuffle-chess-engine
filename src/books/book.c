#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "book.h"

FILE *file;
int book_len;
char** lines;
char* current_line;
int using_book;

void open_book() {
    srand(time(NULL));
    file = fopen("src/books/book.txt", "r");
    if (!file) {
        perror("Opening Book not found!");
        exit(1);
    }
}

void read_book() {
    if (!file) {
        perror("No Opening Book loaded");
        exit(1);
    }
    book_len = 0;
    char line[150];
    char**temp = NULL;
    // lines = (char**)malloc(sizeof(char*));
    while (fgets(line, sizeof(line), file)) {
        temp = (char**)malloc((book_len+1)*sizeof(char*));
        for (int i=0; i<book_len; i++) {
            temp[i] = lines[i];
        }
        lines = (char**)realloc(lines,(book_len+1)*sizeof(char*));
        for (int i = 0; i < book_len; i++) {
            lines[i] = temp[i];
        }
        lines[book_len] = strdup(line);
        book_len++;
        free(temp);
    }
} 

void display_book() {
    for (int i=0;i<book_len;i++) {
        printf("%s",lines[i]);
    }
}

void close_book() {
    if (file) {
        fclose(file);
    }
    file = NULL;
}

void get_book_move(char* c_line) {
    // printf("--%s--\n", c_line);
    if (!using_book) {
        return;
    }
    int match_count = 0;
    char** matches = NULL;
    char**temp = NULL;
    char *arg = NULL;
    for (int i=0;i<book_len;i++) {
        arg = strstr(lines[i], c_line);
        if (arg != NULL && arg == lines[i]) {
            temp = (char**)malloc((match_count+1)*sizeof(char*));
            for (int i=0; i<match_count; i++) {
                temp[i] = matches[i];
            }
            matches = (char**)malloc((match_count+1)*sizeof(char*));
            for (int i = 0; i < match_count; i++) {
                matches[i] = temp[i];
            }
            matches[match_count] = lines[i];
            match_count++;
            free(temp);
        }
    }
    if (match_count){
        int idx = rand() % match_count;
        char* next_line = matches[idx] + strlen(c_line);
        printf("bestmove ");
        if (next_line[0] == ' ') {    
            for (int i = 1; next_line[i] != '\0' && !isspace(next_line[i]); i++) {
                printf("%c",next_line[i]);        
            }
        } else {
            for (int i = 0; next_line[i] != '\0' && !isspace(next_line[i]); i++) {
                printf("%c",next_line[i]);        
            }
        }
        printf("\n");
    } else {
        using_book = 0;
    }
}

void init_book(){
    open_book();
    read_book();
    using_book = 1;
}