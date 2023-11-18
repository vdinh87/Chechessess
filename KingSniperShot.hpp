#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingSniperShot : public Ability
{
private:
    // std::string name = "King Sniper Shot";
    // const int cooldown = 10;
    // const int activation_turn = 0;
public:
    KingSniperShot(SuperChessGame& game_, Logger& log_);
    ~KingSniperShot() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

