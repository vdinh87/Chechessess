#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingTeleport : public Ability
{
private:
    // std::string name = "King Teleport";
    // const int cooldown = 5;
    // const int activation_turn = 0;
public:
    KingTeleport(SuperChessGame& game_, Logger& log_);
    ~KingTeleport() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

