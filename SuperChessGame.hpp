#pragma once
#include "AbilityLibrary.fwd.hpp"
#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
class SuperChessGame : public ChessGame
{
private:
    AbilityLibrary *al;
    std::vector<std::shared_ptr<Ability>> all_abilities;
    U64 SuperPieceBoard;
public:
    SuperChessGame(const SuperPieceInfo &white);
    ~SuperChessGame();
    bool AddPiece(Square square, Color color, Piece piece);
    bool RemovePiece(Square square);
    bool AddSuperPiece(SuperPieceInfo info, Square square, Color color);
    bool AddSuperPiecesofType(SuperPieceInfo info, Color color);
    
    std::vector<Action> Move(Square from_sq, Square to_sq);
    void UseAbility();
};