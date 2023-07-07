#include "Rook.hpp"

Rook::Rook(U64 position_):
Piece(position_)
{
    init_sliders_attacks(rook);
}

U64 Rook::GetAttacks() const
{
    get_rook_attacks
}