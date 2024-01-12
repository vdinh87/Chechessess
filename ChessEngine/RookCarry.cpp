#pragma once
#include "RookCarry.hpp"

RookCarry::RookCarry(SuperChessGame& game_, Logger& log_) :
Ability("Rook Carry", AbilityType::active, game_, log_, 5, 0)
{
}

bool RookCarry::Effect(const SuperPiece& piece)
{
    
    return true;
}

std::unique_ptr<Ability> RookCarry::Clone() const
{
    return std::make_unique<RookCarry>(*this);
}