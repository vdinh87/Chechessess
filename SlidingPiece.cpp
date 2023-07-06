#include "SlidingPiece.hpp"

// Public
SlidingPiece::SlidingPiece(/* args */)
{
    init_magics();

    // init attack tables
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
}

SlidingPiece::~SlidingPiece()
{
}

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

