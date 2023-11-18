#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnTier2 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnTier2(SuperChessGame& game_, Logger& log_);
    ~PawnTier2() = default;

    std::unique_ptr<Ability> Clone() const override;
};

PawnTier2::PawnTier2(SuperChessGame& game_, Logger& log_):
Ability("PawnTier2", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnTier2::Clone() const
{
    return std::make_unique<PawnTier2>(*this);
}

