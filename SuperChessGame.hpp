#pragma once
#include "ChessGame.cpp"
#include "AbilityLibrary.fwd.hpp"
#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
class SuperChessGame : public ChessGame
{
private:
    AbilityLibrary* al;
    std::vector<Ability> all_abilities;
public:
    SuperChessGame(const SuperPieceInfo& white);
    ~SuperChessGame();
    bool AddPiece(Square square, Color color, Piece piece);
    bool RemovePiece(Square square);
    void Move(Square from_sq, Square to_sq);
    void UseAbility(); 
};