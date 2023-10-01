#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class KingSniperShot : public Ability, public Listener
{
private:
    AbilityType type = active;
<<<<<<< HEAD
=======
    std::string name = "King Sniper Shot";
>>>>>>> 74671688b6bc55b6f48211bc66829fc6b81d06c8
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

