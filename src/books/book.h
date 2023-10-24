#pragma once

#include <stdio.h>
#include <time.h>

extern FILE *file;
extern int book_len;
extern char** lines;

extern int using_book;

extern char* current_line;

void open_book();
void read_book();
void close_book();

void init_book();

void get_book_move(char* c_line);