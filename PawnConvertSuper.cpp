#pragma once
#include "PawnConvertSuper.hpp"

PawnConvertSuper::PawnConvertSuper(SuperChessGame &game_, Logger &log_) : 
Ability("Pawn Convert Super", AbilityType::active, game_, log_)
{
}

bool PawnConvertSuper::OnCapture(SuperPiece &piece, Square to_capture)
{
    const Color color = piece.GetColor(); // gets color
    Square sq = piece.GetSquare();        // gets square.
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    game.RemovePiece(to_capture);
    game.AddPiece(to_capture, color, enemy_piece);
    game.RemovePiece(sq);

    SuperPieceInfo info = std::make_pair(enemy_piece, piece.GetTier());
    game.ConvertToSuperPiece(info, to_capture);
}

std::unique_ptr<Ability> PawnConvertSuper::Clone() const
{
    return std::make_unique<PawnConvertSuper>(*this);
}