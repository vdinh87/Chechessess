#include "Queen.hpp"

Queen::Queen(Square position_) : Piece(position_)
{
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
}

Queen::~Queen()
{

}

U64 Queen::GetAttacks(U64 occupancy_) const
{
    return get_bishop_attacks(GetPosition(), occupancy_) | get_rook_attacks(GetPosition(), occupancy_);
}