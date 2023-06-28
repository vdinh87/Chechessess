#include "Knight.hpp"

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

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

    attacks = (((knights >> 6) | (knights << 10)) & ~FILE_G) |
        (((knights >> 10) | (knights << 6)) & ~FILE_A) |
        (((knights >> 15) | (knights << 17)) & ~FILE_H) |
        (((knights >> 17) | (knights << 15)) & ~FILE_A);

    return attacks;
}



U64 GetAttacks(int square) {
    U64 attacks = 0ULL; //0 64 bits
    U64 knights = 0ULL; //

    // Place knight on board
    knights |= (1ULL << square);

    // Add attacks
    attacks = (((knights >> 6) | (knights << 10)) & ~(FILE_G | FILE_H)) |
        (((knights >> 10) | (knights << 6)) & ~(FILE_A | FILE_B)) |
        (((knights >> 15) | (knights << 17)) & ~FILE_H) |
        (((knights >> 17) | (knights << 15)) & ~FILE_A);

    return attacks;
}