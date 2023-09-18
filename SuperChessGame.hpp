#pragma once
#include "AbilityLibrary.fwd.hpp"
#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
class SuperChessGame : public ChessGame
{
private:
    AbilityLibrary *al;
    std::vector<std::shared_ptr<Ability>> all_abilities;

public:
    SuperChessGame(const SuperPieceInfo &white);
    ~SuperChessGame();
    bool AddPiece(Square square, Color color, Piece piece);
    bool RemovePiece(Square square);
    std::vector<Action> Move(Square from_sq, Square to_sq);
    void UseAbility();
};