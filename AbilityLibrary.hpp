#pragma once

#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "KingTurnIntoDead.cpp"
#include "KingSniperShot.cpp"
#include "KingTeleport.cpp"
#include "KingConvert.cpp"
#include "BishopResurrect.cpp"
#include "BishopSwap.cpp"
#include "KnightBigL.cpp"
#include "RookSwap.cpp"
#include "Definitions.hpp"


class AbilityLibrary{
private:
    SuperChessGame* game;
    std::unordered_map<SuperPieceInfo, std::unique_ptr<Ability>, PairEnumHash> lib;
    const std::vector<Tier> MAX_TIERS = {Tier::T3, Tier::T2, Tier::T2, Tier::T2, Tier::T1, Tier::T4};
public:
    AbilityLibrary(SuperChessGame* game_);
    ~AbilityLibrary() = default;
    std::unique_ptr<Ability> GetAbility(const SuperPieceInfo& key);
    Tier GetMaxTier(const Piece p_type) const;
};