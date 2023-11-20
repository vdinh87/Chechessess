#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnTwoTilesBackwards : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnTwoTilesBackwards(SuperChessGame& game_, Logger& log_);
    ~PawnTwoTilesBackwards() = default;

    std::unique_ptr<Ability> Clone() const override;
};

PawnTwoTilesBackwards::PawnTwoTilesBackwards(SuperChessGame& game_, Logger& log_):
Ability("PawnTwoTilesBackwards", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnTwoTilesBackwards::Clone() const
{
    return std::make_unique<PawnTwoTilesBackwards>(*this);
}

