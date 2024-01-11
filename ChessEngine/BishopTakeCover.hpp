#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class BishopTakeCover : public Ability
{
private:
    // std::string name = "Bishop Resurrect";
    // const int cooldown = 0;
    // const int activation_turn = 0;
public:
    BishopTakeCover(SuperChessGame& game_, Logger& log_);
    ~BishopTakeCover() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

