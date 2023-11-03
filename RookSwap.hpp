#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class RookSwap : public Ability, public Listener
{
private:
    std::string name = "Rook Swap";
    std::vector<ChessMove> log_;
    const int cooldown = 2;
    const int activation_turn = 2;
    int cooldown_tracker = 2;
public:
    RookSwap(SuperChessGame* game_);
    ~RookSwap() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

