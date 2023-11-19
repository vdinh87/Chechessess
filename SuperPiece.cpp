#pragma once
#include "SuperPiece.hpp"

SuperPiece::SuperPiece(std::vector<std::unique_ptr<Ability>> &abilities_, SuperPieceInfo info_, Square sq_, Color color_) : 
info(info_), sq(sq_), color(color_)
{
    AddAbilities(abilities_);
}

bool SuperPiece::UseAbility(Tier key)
{
    auto it = abilities.find(key);
    if (it != abilities.end())
    {
        return it->second->Effect(*this);
    }

    return false;
}

void SuperPiece::ModifyMove(U64& move)
{
    for(const auto& pair : abilities)
    {
        if( pair.second->GetType() == AbilityType::move_modifer )
            pair.second->Modify(move, sq);
    }
}

//updaters
void SuperPiece::UpdateSquare(Square to_sq)
{
    sq = to_sq;
}

void SuperPiece::UpdateColor(Color new_color)
{
    color = new_color;
}

void SuperPiece::UpdateTier(Tier t)
{
    info.second = t;
}

void SuperPiece::AddAbilities(std::vector<std::unique_ptr<Ability>> &abilities_)
{
    int i = 0;
    for (auto &a : abilities_)
    {
        abilities[static_cast<Tier>(i)] = std::move(a);
        ++i;
    }

    abilities_.clear();
}

//getters
SuperPieceInfo SuperPiece::GetInfo() const
{
    return info;
}
Square SuperPiece::GetSquare() const
{
    return sq;
}
Color SuperPiece::GetColor() const
{
    return color;
}

std::vector<std::string> SuperPiece::GetAbilityNames() const
{
    std::vector<std::string> v;
    for(auto& a : abilities)
        v.push_back(a.second->GetName());
    
    return v;
}

int SuperPiece::GetNumberOfAbilities() const
{
    return abilities.size();
}