#pragma once
#include "Definitions.hpp"

// Relevant bits for magic bitboards
extern int rook_rellevant_bits[64];
extern int bishop_rellevant_bits[64];

// Random number generation
extern unsigned int state;

// Function declarations
static unsigned int generate_random_number();
U64 random_U64();
U64 random_fewbits();
int count_bits(U64 bitboard);
int get_ls1b_index(U64 bitboard);
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);
U64 mask_bishop_attacks(int square);
U64 mask_rook_attacks(int square);
U64 bishop_attacks_on_the_fly(int square, U64 block);
U64 rook_attacks_on_the_fly(int square, U64 block);
U64 find_magic(int square, int relevant_bits, int bishop);
void init_magics();
void init_sliders_attacks(int bishop);
U64 get_bishop_attacks(int square, U64 occupancy);
U64 get_rook_attacks(int square, U64 occupancy);

// Global arrays
extern U64 bishop_masks[64];
extern U64 rook_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 rook_attacks[64][4096];