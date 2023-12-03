#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class QueenKamikaze : public Ability
{
private:
    U64 to_explode = 0ULL;
    // std::string name = "Queen Kamikaze";
    // const int cooldown = 0;
    // const int activation_turn = 0;
public:
    QueenKamikaze(SuperChessGame& game_, Logger& log_);
    ~QueenKamikaze() = default;

    bool OnCapture(SuperPiece& piece, Square to_capture, Square from_sq) override;
    std::unique_ptr<Ability> Clone() const override;
};

