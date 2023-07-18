#include "Pawn.hpp"

Pawn::Pawn(Square position_, Color color_) : 
Piece(position_, color_)
{
}

bool Pawn::GetHasMoved() const
{
    return has_moved;
}

U64 Pawn::GetAttacks(const U64 occupancy_) const
{
    U64 attacks, pawn = 0ULL;
    set_bit( pawn, position );

    //white-side moves
    if( GetColor() == white )
    {
        attacks = ( North(pawn) | NorthWest(pawn) | NorthEast(pawn) );

        // Initial 2 square move
        if( !has_moved )
            attacks |= pawn << 16;
    }

    //black-side moves
    if ( GetColor() == black)
    {
        attacks = ( South(pawn) | SouthWest(pawn) | SouthEast(pawn) );

        // Initial 2 square move
        if( !has_moved )
            attacks |= pawn >> 16;
    }
    
    return attacks;
}

U64 Pawn::FilterBlocks(const U64 occupancy_) const
{
    return (GetAttacks(occupancy_) & ~occupancy_);
}

void Pawn::SetPosition(Square new_position) 
{ 
    position = new_position;
    if( !has_moved )
        has_moved = true;
}
