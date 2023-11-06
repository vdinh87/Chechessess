#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class BishopTakeCover : public Ability, public Listener
{
private:
    std::string name = "Bishop Resurrect";
    std::vector<ChessMove> log_;
    const int cooldown = 0;
    const int activation_turn = 0;
    int cooldown_tracker = 0;
public:
    BishopTakeCover(SuperChessGame* game_);
    ~BishopTakeCover() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

