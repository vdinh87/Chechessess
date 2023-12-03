#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingInspire : public Ability
{
private:
    Color color;
    // std::string name = "King Inspire";
    // const int cooldown = 0;
    // const int activation_turn = 0;
public:
    KingInspire(SuperChessGame& game_, Logger& log_);
    ~KingInspire() = default;

    void OnGameStart(SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

