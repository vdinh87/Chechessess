#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class RookSwap : public Ability
{
private:
    // std::string name = "Rook Swap";
    // const int cooldown = 2;
    // const int activation_turn = 2;
public:
    RookSwap(SuperChessGame& game_, Logger& log_);
    ~RookSwap() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

