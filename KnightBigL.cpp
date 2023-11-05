#pragma once
#include "KnightBigL.hpp"

KnightBigL::KnightBigL(SuperChessGame *game_) : 
Ability("Knight Big L", AbilityType::move_modifer, game_)
{
}

void KnightBigL::Modify(U64& move, Square piece)
{
    U64 knight = 1ULL << piece;
    move |= (((knight & (FILE_F | FILE_G | FILE_H)) ? 0ULL : (knight >> 13) | (knight << 19)) |
               ((knight & (FILE_A | FILE_B | FILE_C)) ? 0ULL : (knight >> 19) | (knight << 13)) |
               ((knight & (FILE_G | FILE_H)) ? 0ULL : (knight >> 22) | (knight << 26)) |
               ((knight & (FILE_A | FILE_B)) ? 0ULL : (knight >> 26) | (knight << 22)));
}

std::unique_ptr<Ability> KnightBigL::Clone() const
{
    return std::make_unique<KnightBigL>(*this);
}