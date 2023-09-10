#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingSniperShot : public Ability
{
private:
    AbilityType type = active;
    std::string name = "King Sniper Shot";
    SuperChessGame* game;
public:
    KingSniperShot(SuperChessGame* game_);
    ~KingSniperShot() = default;
    void Effect();
};

