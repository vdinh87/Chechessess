#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class KingTurnIntoDead : public Ability, public Listener
{
private:
    std::string name = "King Turn Into Dead";
    std::vector<ChessMove> log_;
    const int cooldown = 5;
    const int activation_turn = 0;
    int cooldown_tracker = 5;
public:
    KingTurnIntoDead(SuperChessGame* game_);
    ~KingTurnIntoDead() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

