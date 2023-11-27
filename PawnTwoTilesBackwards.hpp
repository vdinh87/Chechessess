#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class PawnTwoTilesBackwards : public Ability
{
public:
    PawnTwoTilesBackwards(SuperChessGame &game_, Logger &log_);
    ~PawnTwoTilesBackwards() = default;

    void Modify(U64 &move, Square piece) override;
    std::unique_ptr<Ability> Clone() const override;
};
