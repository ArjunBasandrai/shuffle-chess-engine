#ifndef STDIO_H_
#define STDIO_H_
#include <stdio.h>
#endif

#include <string.h>
#include "helpers/magic_num_generator.h"

#ifndef CONST_H_
#define CONST_H_
#include "helpers/board_constants.h"
#endif

#ifndef BITS_H_
#define BITS_H_
#include "helpers/bit_manipulation.h"
#endif

#ifndef MASKS_H_
#define MASKS_H_
#include "helpers/masks.h"
#endif

#ifndef MAGIC_NUMS_H_
#define MAGIC_NUMS_H_
#include "helpers/magic_numbers.h"
#endif

#ifndef U64
#define U64 unsigned long long
#endif

#include "helpers/pre_calculated_tables.h"


void init_all() {
    init_leaper_attacks();
    // init_magic_numbers();
}

// Main driver
int main(){
    init_all();
    return 0;
}