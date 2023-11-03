#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class BishopResurrect : public Ability, public Listener
{
private:
    std::string name = "Bishop Resurrect";
    std::vector<ChessMove> log_;
    const int cooldown = 6;
    const int activation_turn = 8;
    int cooldown_tracker = 6;
public:
    BishopResurrect(SuperChessGame* game_);
    ~BishopResurrect() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

