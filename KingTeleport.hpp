#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class KingTeleport : public Ability, public Listener
{
private:
    AbilityType type = active;
    std::string name = "King Teleport";
    std::vector<ChessMove> log_;
    const int cooldown = 5;
    int cooldown_tracker = 5;
public:
    KingTeleport(SuperChessGame* game_);
    ~KingTeleport() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

