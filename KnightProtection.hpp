#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KnightProtection : public Ability
{
private:
    bool can_protect = true;
    // std::string name = "King Protection";
    // const int cooldown = 0;
    // const int activation_turn = 0;
public:
    KnightProtection(SuperChessGame& game_, Logger& log_);
    ~KnightProtection() = default;

    bool OnCapture(SuperPiece& piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove) override;
    std::unique_ptr<Ability> Clone() const override;
};

