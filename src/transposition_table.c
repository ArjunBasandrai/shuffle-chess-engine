#include <stdlib.h>

#include "transposition_table.h"

#ifndef U64
#define U64 unsigned long long
#endif

int hash_entries = 0;
tt *transposition_table = NULL;

void clear_transposition_table() {
    tt *hash_entry;
    for (hash_entry = transposition_table; hash_entry < transposition_table + hash_entries; hash_entry++) {
        hash_entry->hash_key = 0;
        hash_entry->depth = 0;
        hash_entry->flag = 0;
        hash_entry->score = 0;
    }
}

void init_transposition_table(int mb) {
    int hash_size = 0x100000 * mb;  
    hash_entries = hash_size / sizeof(tt);

    if (transposition_table != NULL) {
        // printf("Clearing tt memory...\n");
        free(transposition_table);
    }

    transposition_table = (tt*)malloc(hash_entries*sizeof(tt));

    if (transposition_table == NULL) {
        // printf("Couldn't allocate memory for tt, trying size %dMB\n"),mb/2;
        init_transposition_table(mb/2);
    } else {
        clear_transposition_table();
        // printf("tt is initialized with %d entries\n", hash_entries);
    }
}