#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class QueenInspire : public Ability
{
private:
    Color color;
    // std::string name = "Queen Inspire";
    // const int cooldown = 0;
    // const int activation_turn = 0;
public:
    QueenInspire(SuperChessGame& game_, Logger& log_);
    ~QueenInspire();

    void OnGameStart(SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

