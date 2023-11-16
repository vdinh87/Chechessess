#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnTier3 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnTier3(SuperChessGame& game_, Logger& log_);
    ~PawnTier3() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

PawnTier3::PawnTier3(SuperChessGame& game_, Logger& log_):
Ability("PawnTier3", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnTier3::Clone() const
{
    return std::make_unique<PawnTier3>(*this);
}

