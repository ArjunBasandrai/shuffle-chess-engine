#include <stdio.h>
#include <string.h>

#include "magic_num_generator.h"

#include "bit_manipulation.h"
#include "masks.h"
#include "magic_numbers.h"
#include "random_unsigned.h"

U64 generate_magic_number() {
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {
    U64 occupancy = 0ULL;

    for (int count = 0; count < bits_in_mask; count++){
        int square = get_lsb_index(attack_mask);
        pop_bit(attack_mask,square);
        if (index & (1ULL << count)) {
            occupancy |= (1ULL << square);
        }
    }

    return occupancy;
}

// Magic numbers

U64 find_magic_number(int square, int relevant_bits, int bishop)
{
    U64 occupancies[4096];
    
    U64 attacks[4096];
    
    U64 used_attacks[4096];
    
    U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);
    
    int occupancy_indicies = 1 << relevant_bits;
    
    for (int index = 0; index < occupancy_indicies; index++)
    {
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);
        
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) :
                                    rook_attacks_on_the_fly(square, occupancies[index]);
    }
    
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        U64 magic_number = generate_magic_number();
        
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        memset(used_attacks, 0ULL, sizeof(used_attacks));
        
        int index, fail;
        
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));
            
            if (used_attacks[magic_index] == 0ULL)
                used_attacks[magic_index] = attacks[index];
            
            else if (used_attacks[magic_index] != attacks[index])
                fail = 1;
        }
        
        if (!fail)
            return magic_number;
    }
    
    printf("  Magic number fails!\n");
    return 0ULL;
}

// void init_magic_numbers()
// {
//     for (int square = 0; square < 64; square++)
//         rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);
//     for (int square = 0; square < 64; square++)
//         bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
// }
