#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KnightTier1 : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    KnightTier1(SuperChessGame& game_, Logger& log_);
    ~KnightTier1() = default;

    std::unique_ptr<Ability> Clone() const override;
};

KnightTier1::KnightTier1(SuperChessGame& game_, Logger& log_):
Ability("KnightTier1", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> KnightTier1::Clone() const
{
    return std::make_unique<KnightTier1>(*this);
}

