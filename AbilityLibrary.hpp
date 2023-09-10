#pragma once

#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "KingSniperShot.cpp"
#include "Definitions.hpp"

class AbilityLibrary{
private:
    SuperChessGame* game;
    std::unordered_map<SuperPieceInfo, Ability, PairEnumHash> lib;
public:
    AbilityLibrary(SuperChessGame* game_);
    ~AbilityLibrary() = default;
    Ability GetAbility(const SuperPieceInfo& key);
};