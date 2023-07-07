#include "SlidingPiece.hpp"

// Public
SlidingPiece::SlidingPiece(/* args */)
{
    // init attack tables
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
}

SlidingPiece::~SlidingPiece()
{
}

U64 SlidingPiece::get_bishop_attacks(int square, U64 occupancy)
{       
    // calculate magic index
    occupancy &= bishop_masks[square];
    occupancy *=  bishop_magics[square];
    occupancy >>= 64 - bishop_rellevant_bits[square];
    
    // return rellevant attacks
    return bishop_attacks[square][occupancy];  
}

U64 SlidingPiece::get_rook_attacks(int square, U64 occupancy) 
{        
    // calculate magic index
    occupancy &= rook_masks[square];
    occupancy *=  rook_magics[square];
    occupancy >>= 64 - rook_rellevant_bits[square];

    // return rellevant attacks
    return rook_attacks[square][occupancy];
}

//Private
unsigned int SlidingPiece::generate_random_number()
{
    // XOR shift algorithm
    unsigned int x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}

U64 SlidingPiece::random_U64()
{
    // init numbers to randomize
    U64 u1, u2, u3, u4;
    
    // randomize numbers
    u1 = (U64)(generate_random_number()) & 0xFFFF;
    u2 = (U64)(generate_random_number()) & 0xFFFF;
    u3 = (U64)(generate_random_number()) & 0xFFFF;
    u4 = (U64)(generate_random_number()) & 0xFFFF;
    
    // shuffle bits and return
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

U64 SlidingPiece::random_fewbits() 
{
    return random_U64() & random_U64() & random_U64();
}

int SlidingPiece::count_bits(U64 bitboard) 
{
    // bit count
    int count = 0;
    
    // pop bits untill bitboard is empty
    while (bitboard)
    {
        // increment count
        count++;
        
        // consecutively reset least significant 1st bit
        bitboard &= bitboard - 1;
    }
    
    // return bit count
    return count;
}

// get index of LS1B in bitboard
int SlidingPiece::get_ls1b_index(U64 bitboard) 
{
    // make sure bitboard is not empty
    if (bitboard != 0)
        // convert trailing zeros before LS1B to ones and count them
        return count_bits((bitboard & -bitboard) - 1);
    
    // otherwise
    else
        // return illegal index
        return -1;
}

// set occupancies
U64 SlidingPiece::set_occupancy(int index, int bits_in_mask, U64 attack_mask)
{
    // occupancy map
    U64 occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);
        
        // pop LS1B in attack map
        pop_bit(attack_mask, square);
        
        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    
    // return occupancy map
    return occupancy;
}

// mask bishop attacks
U64 SlidingPiece::mask_bishop_attacks(int square)
{
    // attack bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

// mask rook attacks
U64 SlidingPiece::mask_rook_attacks(int square)
{
    // attacks bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

U64 SlidingPiece::bishop_attacks_on_the_fly(int square, U64 block)
{
    // attack bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    // return attack map for bishop on a given square
    return attacks;
}

U64 SlidingPiece::rook_attacks_on_the_fly(int square, U64 block)
{
    // attacks bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    // return attack map for bishop on a given square
    return attacks;
}

/**************************************\
        Generating magic numbers
        
\**************************************/

// find magic number
U64 SlidingPiece::find_magic(int square, int relevant_bits, int bishop) 
{
    // define occupancies array
    U64 occupancies[4096];

    // define attacks array
    U64 attacks[4096];

    // define used indicies array
    U64 used_attacks[4096];
    
    // mask piece attack
    U64 mask_attack = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    // occupancy variations
    int occupancy_variations = 1 << relevant_bits;

    // loop over the number of occupancy variations
    for(int count = 0; count < occupancy_variations; count++) {
        // init occupancies
        occupancies[count] = set_occupancy(count, relevant_bits, mask_attack);
        
        // init attacks
        attacks[count] = bishop ? bishop_attacks_on_the_fly(square, occupancies[count]) :
                                rook_attacks_on_the_fly(square, occupancies[count]);
    }

    // test magic numbers
    for(int random_count = 0; random_count < 100000000; random_count++)
    {
        // init magic number candidate
        U64 magic = random_fewbits();

        // skip testing magic number if inappropriate
        if(count_bits((mask_attack * magic) & 0xFF00000000000000ULL) < 6) continue;

        // reset used attacks array
        memset(used_attacks, 0ULL, sizeof(used_attacks));
        
        // init count & fail flag
        int count, fail;
        
        // test magic index
        for (count = 0, fail = 0; !fail && count < occupancy_variations; count++) {
            // generate magic index
            int magic_index = (int)((occupancies[count] * magic) >> (64 - relevant_bits));
        
            // if got free index
            if(used_attacks[magic_index] == 0ULL)
                // assign corresponding attack map
                used_attacks[magic_index] = attacks[count];

            // otherwise fail on  collision
            else if(used_attacks[magic_index] != attacks[count]) fail = 1;
        }
        
        // return magic if it works
        if(!fail) return magic;
    }
    
    // on fail
    printf("***Failed***\n");
    return 0ULL;
}

void SlidingPiece::init_magics()
{
    printf("const U64 rook_magics[64] = {\n");
    
    // loop over 64 board squares
    for(int square = 0; square < 64; square++)
        printf("    0x%llxULL,\n", find_magic(square, rook_rellevant_bits[square], 0));
    
    printf("};\n\nconst U64 bishop_magics[64] = {\n");
    
    // loop over 64 board squares
    for(int square = 0; square < 64; square++)
        printf("    0x%llxULL,\n", find_magic(square, bishop_rellevant_bits[square], 1));
    
    printf("};\n\n");
}

// init slider pieces attacks
void SlidingPiece::init_sliders_attacks(int is_bishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);
        
        // init current mask
        U64 mask = is_bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);
        
        // count attack mask bits
        int bit_count = count_bits(mask);
        
        // occupancy variations count
        int occupancy_variations = 1 << bit_count;
        
        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
            // bishop
            if (is_bishop)
            {
                // init occupancies, magic index & attacks
                U64 occupancy = set_occupancy(count, bit_count, mask);
                U64 magic_index = occupancy * bishop_magics[square] >> 64 - bishop_rellevant_bits[square];
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);                
            }
            
            // rook
            else
            {
                // init occupancies, magic index & attacks
                U64 occupancy = set_occupancy(count, bit_count, mask);
                U64 magic_index = occupancy * rook_magics[square] >> 64 - rook_rellevant_bits[square];
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);                
            }
        }
    }
}