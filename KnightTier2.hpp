#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KnightTier2 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    KnightTier2(SuperChessGame& game_, Logger& log_);
    ~KnightTier2() = default;

    std::unique_ptr<Ability> Clone() const override;
};

KnightTier2::KnightTier2(SuperChessGame& game_, Logger& log_):
Ability("KnightTier2", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> KnightTier2::Clone() const
{
    return std::make_unique<KnightTier2>(*this);
}

