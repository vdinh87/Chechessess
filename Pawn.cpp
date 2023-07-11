#include "Pawn.hpp"

Pawn::Pawn(Square position_, Color color_) : 
Piece(position_, color_)
{
}

U64 Pawn::GetAttacks(U64 occupancy_) const
{
    U64 attacks, pawn = 0ULL;
    set_bit( pawn, GetPosition() );

    //white-side moves
    if( GetColor() == white )
    {
        attacks = ( North(pawn) | NorthWest(pawn) | NorthEast(pawn) );
        if( !has_moved )
            attacks |= pawn << 16;
    }

    //black-side moves
    if ( GetColor() == black)
    {
        attacks = ( South(pawn) | SouthWest(pawn) | SouthEast(pawn) );
        if( !has_moved )
            attacks |= pawn >> 16;
    }
    
    return attacks;
}
