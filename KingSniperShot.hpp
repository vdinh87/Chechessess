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
    const int activation_turn = 10;
    int cooldown_tracker = 10;
public:
    KingSniperShot(SuperChessGame* game_);
    ~KingSniperShot() = default;
    void Effect();
    void Notify(const std::vector<ChessMove>& log) override;
};

