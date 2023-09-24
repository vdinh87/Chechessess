#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class KingSniperShot : public Ability, public Listener
{
private:
    AbilityType type = active;
    std::string name = "King Sniper Shot";
    SuperChessGame* game;
    std::vector<ChessMove> log_;
public:
    KingSniperShot(SuperChessGame* game_);
    ~KingSniperShot() = default;
    void Effect();
    void Notify(const std::vector<ChessMove>& log) override;
};

