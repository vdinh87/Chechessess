#pragma once
#include "SuperPiece.hpp"

SuperPiece::SuperPiece(std::vector<std::unique_ptr<Ability>>& abilities_, SuperPieceInfo info_, Square sq_, Color color_) :
info(info_), 
sq(sq_),
color(color_)
{
    for(auto& a : abilities_)
    {
        abilities[info_.second] = std::move(a);
    }
    
    abilities_.clear();
}

bool SuperPiece::UseAbility(Tier key)
{
    auto it = abilities.find(key);
    if(it != abilities.end())
    {
        it->second->Effect(*this);
        return true;
    }

    return false;
}

void SuperPiece::UpdateSquare(Square to_sq)
{
    sq = to_sq;
}

const SuperPieceInfo& SuperPiece::GetInfo() const
{
    return info;
}
const Square& SuperPiece::GetSquare() const
{
    return sq;
}
const Color& SuperPiece::GetColor() const
{
    return color;
}