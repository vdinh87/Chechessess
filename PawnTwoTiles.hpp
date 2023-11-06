#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class PawnTwoTiles : public Ability
{
private:
    std::string name = "Pawn Two Tiles";
public:
    PawnTwoTiles(SuperChessGame* game_);
    ~PawnTwoTiles() = default;

    void Modify(U64& move, Square piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

