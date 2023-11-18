#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingTurnIntoDead : public Ability
{
private:
    // std::string name = "King Turn Into Dead";
    // const int cooldown = 5;
    // const int activation_turn = 0;
public:
    KingTurnIntoDead(SuperChessGame& game_, Logger& log_);
    ~KingTurnIntoDead() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

