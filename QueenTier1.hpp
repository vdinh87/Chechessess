#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class QueenTier1 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    QueenTier1(SuperChessGame& game_, Logger& log_);
    ~QueenTier1() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

QueenTier1::QueenTier1(SuperChessGame& game_, Logger& log_):
Ability("QueenTier1", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> QueenTier1::Clone() const
{
    return std::make_unique<QueenTier1>(*this);
}

