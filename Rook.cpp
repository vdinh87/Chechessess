#include "Rook.hpp"

Rook::Rook(Square position_, Color color_) : 
Piece(position_, color_)
{
    init_sliders_attacks(rook);
}

U64 Rook::GetAttacks(U64 occupancy_) const
{
    return get_rook_attacks(GetPosition(), occupancy_);
}