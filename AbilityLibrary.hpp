#pragma once

#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "KingSniperShot.cpp"
#include "Definitions.hpp"
template <typename T>
void callDerivedMethod(std::unordered_map<SuperPieceInfo, std::shared_ptr<Ability>, PairEnumHash> &lib, const SuperPieceInfo &key);

class AbilityLibrary{
private:
    SuperChessGame* game;
    std::unordered_map<SuperPieceInfo, std::shared_ptr<Ability>, PairEnumHash> lib;
public:
    AbilityLibrary(SuperChessGame* game_);
    ~AbilityLibrary() = default;
    std::shared_ptr<Ability> GetAbility(const SuperPieceInfo& key);
};