#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class BishopResurrect : public Ability, public Listener
{
private:
    std::vector<U64> WhiteSpawnLocations{
            0x000000000000FF00ULL, 
            0x0000000000000042ULL, 
            0x0000000000000024ULL, 
            0x0000000000000081ULL,
            0x0000000000000008ULL, 
            0x0000000000000010ULL };

       std::vector<U64> BlackSpawnLocations{
            0x00FF000000000000ULL,
            0x4200000000000000ULL,
            0x2400000000000000ULL,
            0x8100000000000000ULL,
            0x0800000000000000ULL,
            0x1000000000000000ULL};


    AbilityType type = active;
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

