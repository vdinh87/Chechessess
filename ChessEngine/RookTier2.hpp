#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class RookTier2 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    RookTier2(SuperChessGame& game_, Logger& log_);
    ~RookTier2() = default;

    std::unique_ptr<Ability> Clone() const override;
};

RookTier2::RookTier2(SuperChessGame& game_, Logger& log_):
Ability("RookTier2", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> RookTier2::Clone() const
{
    return std::make_unique<RookTier2>(*this);
}

