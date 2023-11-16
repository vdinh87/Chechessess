#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
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

    void OnCapture(SuperPiece& piece, SuperPiece& other_piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
    
    bool HasBuff(SuperPiece& piece) const;
};

