#pragma once
#include "SuperPiece.hpp"

SuperPiece::SuperPiece(std::vector<std::unique_ptr<Ability>>& abilities_, SuperPieceInfo info_, Square sq_, Color color_) :
info(info_), 
sq(sq_),
color(color_)
{
    for(auto& a : abilities_)
        abilities.push_back(std::move(a));
    
    abilities_.clear();
}

void SuperPiece::UseAbility()
{
    for(auto& a : abilities)
    {
        a->Effect();
    }
}
