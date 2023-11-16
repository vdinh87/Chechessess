#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnTier1 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnTier1(SuperChessGame& game_, Logger& log_);
    ~PawnTier1() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

PawnTier1::PawnTier1(SuperChessGame& game_, Logger& log_):
Ability("PawnTier1", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnTier1::Clone() const
{
    return std::make_unique<PawnTier1>(*this);
}

