#pragma once
#include "AbilityLibrary.hpp"

AbilityLibrary::AbilityLibrary(SuperChessGame* game_)
{
    game = game_;
    lib[std::make_pair(King, T3)] = KingSniperShot(game_);
}

Ability AbilityLibrary::GetAbility(const SuperPieceInfo& key)
{
    return lib[key];
}
