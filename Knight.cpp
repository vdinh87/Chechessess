#include "Knight.hpp"

Knight::Knight(U64 position_):
Piece(position_)
{
}

Knight::~Knight()
{
}

U64 Knight::GetAttacks() const
{//Will get the possible attack locations for knights.
    U64 attacks = 0ULL;
    U64 knights = GetPosition();
    attacks = (((knights >> 6) | (knights << 10)) & ~(FILE_G | FILE_H)) |
        (((knights >> 10) | (knights << 6)) & ~(FILE_A | FILE_B)) |
        (((knights >> 15) | (knights << 17)) & ~FILE_H) |
        (((knights >> 17) | (knights << 15)) & ~FILE_A);

    return attacks;   //Queried with U64 attacks = Knightattacks[from] & targets;
}
