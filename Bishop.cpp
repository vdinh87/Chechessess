#include "Bishop.hpp"

Bishop::Bishop(Square position_) : Piece(position_)
{
    init_sliders_attacks(bishop);
}

Bishop::~Bishop()
{

}

U64 Bishop::GetAttacks(U64 occupancy_) const
{
    return get_bishop_attacks(GetPosition(), occupancy_);
}