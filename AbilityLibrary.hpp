#pragma once

#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "KingSniperShot.cpp"
#include "Definitions.hpp"

class AbilityLibrary{
private:
    SuperChessGame* game;
    std::unordered_map<SuperPieceInfo, std::unique_ptr<Ability>, PairEnumHash> lib;
public:
    AbilityLibrary(SuperChessGame* game_);
    ~AbilityLibrary() = default;
    std::unique_ptr<Ability> GetAbility(const SuperPieceInfo& key);
};