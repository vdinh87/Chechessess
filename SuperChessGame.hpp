#pragma once
#include "AbilityLibrary.fwd.hpp"
#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "SuperPiece.hpp"
class SuperChessGame : public ChessGame
{
private:
    // ability related data members
    std::shared_ptr<AbilityLibrary> al;
    std::unordered_map<Square, std::shared_ptr<SuperPiece>> super_pieces;

    //init
    void InitSuperPieces(const SuperPieceInfo &white, const SuperPieceInfo &black);
public:
    SuperChessGame(const SuperPieceInfo &white, const SuperPieceInfo &black);
    ~SuperChessGame() = default;

    bool RemovePiece(Square square) override;
    bool AddSuperPiece(SuperPieceInfo info, Square square, Color color, bool conversion = 0);
    bool AddSuperPiecesofType(SuperPieceInfo info, Color color);
    bool ConvertPieceToSide(Square square, Color side);
    bool ConvertToSuperPiece(SuperPieceInfo info, Square square);
    bool UpgradeSuperPieceTier(Square square, Tier to_tier);


    //utility
    bool IsSuperPiece(const Square& key) const;
    bool InCheck(Color color) const;
    void CapTier(Tier& t, Piece p_type) const;
    void MakeAbilityVector(std::vector<std::unique_ptr<Ability>>& v, SuperPieceInfo info);
    // Move functions
    std::vector<Action> Move(Square from_sq, Square to_sq) override;
    void ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece) override;
    
    //misc
    void Do(Square sq, Tier t);
};