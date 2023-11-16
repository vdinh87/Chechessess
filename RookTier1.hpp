#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class RookTier1 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    RookTier1(SuperChessGame& game_, Logger& log_);
    ~RookTier1() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

RookTier1::RookTier1(SuperChessGame& game_, Logger& log_):
Ability("RookTier1", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> RookTier1::Clone() const
{
    return std::make_unique<RookTier1>(*this);
}

