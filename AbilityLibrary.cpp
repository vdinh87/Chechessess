#pragma once
#include "AbilityLibrary.hpp"

AbilityLibrary::AbilityLibrary(SuperChessGame *game_)
{
    game = game_;
    lib[std::make_pair(King, T3)] = std::make_shared<KingSniperShot>(game_);
}

std::shared_ptr<Ability> AbilityLibrary::GetAbility(const SuperPieceInfo &key)
{
    return lib[key];
}

template <typename T>
void callDerivedMethod(std::unordered_map<SuperPieceInfo, std::shared_ptr<Ability>, PairEnumHash> &lib, const SuperPieceInfo &key)
{
    auto it = lib.find(key);
    if (it != lib.end())
    {
        std::shared_ptr<Ability> ability = it->second;

        if (auto derivedPtr = std::dynamic_pointer_cast<T>(ability))
        {
            derivedPtr->Effect();
        }
    }
}