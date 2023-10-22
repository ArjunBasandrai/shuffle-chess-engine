#pragma once

#ifndef U64
#define U64 unsigned long long
#endif

// Random numbers
extern unsigned int random_state;

unsigned int get_random_U32_number();

U64 get_random_U64_number();