#ifndef U64
#define U64 unsigned long long
#endif

#define hash_size 0x400000

#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

typedef struct {
    U64 hash_key;
    int depth;
    int flag;
    int score;

} tt;

tt transposition_table[hash_size];

void clear_transposition_table() {
    for (int index = 0; index < hash_size; index++) {
        transposition_table[index].hash_key = 0;
        transposition_table[index].depth = 0;
        transposition_table[index].flag = 0;
        transposition_table[index].score = 0;
    }
}