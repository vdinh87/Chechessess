#pragma once
#include "AbilityLibrary.hpp"

AbilityLibrary::AbilityLibrary(SuperChessGame *game_)
{
    game = game_;
    lib[std::make_pair(King, T1)] = std::make_unique<KingTurnIntoDead>(game_);
    lib[std::make_pair(King, T2)] = std::make_unique<KingTeleport>(game_);
    lib[std::make_pair(King, T3)] = std::make_unique<KingSniperShot>(game_);
    lib[std::make_pair(King, T4)] = std::make_unique<KingConvert>(game_);
    lib[std::make_pair(Bishop, T2)] = std::make_unique<BishopResurrect>(game_);
    lib[std::make_pair(Bishop, T0)] = std::make_unique<BishopSwap>(game_);
    lib[std::make_pair(Bishop, T0)] = std::make_unique<RookSwap>(game_);

}

std::unique_ptr<Ability> AbilityLibrary::GetAbility(const SuperPieceInfo &key)
{
    auto it = lib.find(key);
    if (it != lib.end()) 
        return it->second->Clone();

    return nullptr;
}

Tier AbilityLibrary::GetMaxTier(const Piece p_type) const
{
    return MAX_TIERS[p_type];
}