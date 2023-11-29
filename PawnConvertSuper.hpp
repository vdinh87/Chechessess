#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnConvertSuper : public Ability
{
public:
    PawnConvertSuper(SuperChessGame& game_, Logger& log_);
    ~PawnConvertSuper() = default;

    bool OnCapture(SuperPiece& piece, Square to_capture) override;
    std::unique_ptr<Ability> Clone() const override;
};

