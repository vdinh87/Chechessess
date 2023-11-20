#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnConvert : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnConvert(SuperChessGame& game_, Logger& log_);
    ~PawnConvert() = default;

    std::unique_ptr<Ability> Clone() const override;
};

PawnConvert::PawnConvert(SuperChessGame& game_, Logger& log_):
Ability("PawnConvert", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnConvert::Clone() const
{
    return std::make_unique<PawnConvert>(*this);
}


