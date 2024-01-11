#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class RookRamBuff : public Ability
{
private:
    // std::string name = "Rook Ram Buff";
    // const int cooldown = 6;
    // const int activation_turn = 0;
public:
    RookRamBuff(SuperChessGame& game_, Logger& log_);
    ~RookRamBuff() = default;

    bool OnCapture(SuperPiece& piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove) override;
    std::unique_ptr<Ability> Clone() const override;
    
    bool HasBuff(SuperPiece& piece) const;
};

