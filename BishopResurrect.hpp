#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class BishopResurrect : public Ability
{
// private:
//     std::string name = "Bishop Resurrect";
//     const int cooldown = 6;
//     const int activation_turn = 8;
public:
    BishopResurrect(SuperChessGame& game_, Logger& log_);
    ~BishopResurrect() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

