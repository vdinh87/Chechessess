#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnConvertSuper : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    PawnConvertSuper(SuperChessGame& game_, Logger& log_);
    ~PawnConvertSuper() = default;

    std::unique_ptr<Ability> Clone() const override;
};

PawnConvertSuper::PawnConvertSuper(SuperChessGame& game_, Logger& log_):
Ability("PawnConvertSuper", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> PawnConvertSuper::Clone() const
{
    return std::make_unique<PawnConvertSuper>(*this);
}

