#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnConvert : public Ability
{
public:
    PawnConvert(SuperChessGame& game_, Logger& log_);
    ~PawnConvert() = default;

    bool OnCapture(SuperPiece& piece, Square to_capture) override;
    std::unique_ptr<Ability> Clone() const override;
};

