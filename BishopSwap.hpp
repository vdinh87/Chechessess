#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class BishopSwap : public Ability
{
private:
    // std::string name = "Bishop Swap";
    // const int cooldown = 2;
    // const int activation_turn = 2;

    U64 blackSquares = 0xAA55AA55AA55AA55ULL;
public:
    BishopSwap(SuperChessGame& game_, Logger& log_);
    ~BishopSwap() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

