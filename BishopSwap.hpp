#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class BishopSwap : public Ability, public Listener
{
private:
    AbilityType type = active;
    std::string name = "Bishop Swap";
    std::vector<ChessMove> log_;
    const int cooldown = 2;
    const int activation_turn = 2;
    int cooldown_tracker = 2;
    U64 blackSquares = 0xAA55AA55AA55AA55ULL;
public:
    BishopSwap(SuperChessGame* game_);
    ~BishopSwap() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

