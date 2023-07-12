#include "Bishop.hpp"

Bishop::Bishop(Square position_, Color color_) : 
Piece(position_, color_)
{
    init_sliders_attacks(bishop);
}

U64 Bishop::GetAttacks(U64 occupancy_) const
{
    return get_bishop_attacks(GetPosition(), occupancy_);
}