#pragma once
#include "AbilityLibrary.hpp"

AbilityLibrary::AbilityLibrary(SuperChessGame *game_)
{
    game = game_;
    lib[std::make_pair(King, T3)] = std::make_unique<KingSniperShot>(game_);
}

std::unique_ptr<Ability> AbilityLibrary::GetAbility(const SuperPieceInfo &key)
{
    auto it = lib.find(key);
    if (it != lib.end()) 
        return it->second->Clone();

    return nullptr;
}