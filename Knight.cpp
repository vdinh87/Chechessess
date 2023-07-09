#include "Knight.hpp"

Knight::Knight(Square position_, Color color_) : 
Piece(position_, color_)
{
    
}

Knight::~Knight()
{
}

U64 Knight::GetAttacks(U64 occupancy_) const
{//Will get the possible attack locations for knights.
    U64 attacks, knights = 0ULL;
    set_bit( knights, GetPosition() );
    attacks = ( ((knights & (FILE_G | FILE_H)) ? 0ULL : (knights >> 6) | (knights << 10)) | 
                (( knights & (FILE_A | FILE_B)) ? 0ULL : (knights >> 10) | (knights << 6)) |
                (( knights & FILE_H) ? 0ULL : (knights >> 15 | knights << 17)) |
                (( knights & FILE_A) ? 0ULL : (knights >> 17 | knights << 15)) );


    return attacks;   //Queried with U64 attacks = Knightattacks[from] & targets;
}
