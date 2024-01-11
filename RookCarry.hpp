#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class RookCarry : public Ability
{
private:
    // std::string name = "Rook Carry";
    // const int cooldown = 5;
    // const int activation_turn = 0;
public:
    RookCarry(SuperChessGame& game_, Logger& log_);
    ~RookCarry() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

