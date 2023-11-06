#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class PawnTwoTiles : public Ability, public Listener
{
private:
    std::string name = "Pawn Two Tiles";
    std::vector<ChessMove> log_;
public:
    PawnTwoTiles(SuperChessGame* game_);
    ~PawnTwoTiles() = default;

    void Effect(const SuperPiece& piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
};

