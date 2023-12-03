#pragma once
#include "PawnConvertSuper.hpp"

PawnConvertSuper::PawnConvertSuper(SuperChessGame &game_, Logger &log_) : 
Ability("Pawn Convert Super", AbilityType::on_capture, game_, log_)
{
}

bool PawnConvertSuper::OnCapture(SuperPiece &piece, Square to_capture, Square from_sq)
{
    const Color color = piece.GetColor(); // gets color
    Square sq = piece.GetSquare();        // gets square.
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    if ( game.RemovePiece(to_capture) && game.AddPiece(to_capture, color, enemy_piece) && game.RemovePiece(sq) && game.ConvertToSuperPiece(std::make_pair(enemy_piece, piece.GetTier()), to_capture))
        return true;
    
    return false;

}

std::unique_ptr<Ability> PawnConvertSuper::Clone() const
{
    return std::make_unique<PawnConvertSuper>(*this);
}