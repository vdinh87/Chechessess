#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class QueenTier0 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    QueenTier0(SuperChessGame& game_, Logger& log_);
    ~QueenTier0() = default;

    std::unique_ptr<Ability> Clone() const override;
};

QueenTier0::QueenTier0(SuperChessGame& game_, Logger& log_):
Ability("QueenTier0", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> QueenTier0::Clone() const
{
    return std::make_unique<QueenTier0>(*this);
}

