#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class KingSniperShot : public Ability, public Listener
{
private:
    AbilityType type = active;
    std::string name = "King Sniper Shot";
    std::vector<ChessMove> log_;
    const int cooldown = 10;
    const int activation_turn = 0;
    int cooldown_tracker = 10;
public:
    KingSniperShot(SuperChessGame* game_);
    ~KingSniperShot() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

