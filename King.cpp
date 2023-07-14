#include "King.hpp"

King::King(Square position_, Color color_) : 
Piece(position_, color_)
{
}

U64 King::GetAttacks(U64 occupany_) const
{
    U64 attacks, king = 0ULL;
    set_bit(king, position);
    attacks = ( 
                North(king) | NorthEast(king) | NorthWest(king) | 
                South(king) | SouthEast(king) | SouthWest(king) |
                East(king) | West(king)   
                );

    return attacks;
}

bool King::GetHasMoved() const
{
    return has_moved;
}