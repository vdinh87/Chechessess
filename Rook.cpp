#include "Rook.hpp"

Rook::Rook(Square position_, Color color_) : 
Piece(position_, color_)
{
    init_sliders_attacks(rook);
}

bool Rook::GetHasMoved() const
{
    return has_moved;
}

U64 Rook::GetAttacks(const U64 occupancy_) const
{
    return get_rook_attacks(GetPosition(), occupancy_);
}

void Rook::SetPosition(Square new_position) 
{ 
    position = new_position;
    if( !has_moved )
        has_moved = true;
}