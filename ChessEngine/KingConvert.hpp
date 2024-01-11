#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingConvert : public Ability
{
private:
    // std::string name = "King Convert";
    // const int cooldown = 10;
    // const int activation_turn = 15;
public:
    KingConvert(SuperChessGame& game_, Logger& log_);
    ~KingConvert() = default;

    bool Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

