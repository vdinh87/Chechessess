#pragma once
#include "PawnConvertSuper.hpp"

PawnConvertSuper::PawnConvertSuper(SuperChessGame &game_, Logger &log_) : Ability("Pawn Convert Piece", AbilityType::active, game_, log_, 0, 0)
{
}

void PawnConvertSuper::OnCapture(SuperPiece &piece, Square to_capture)
{
    int current_turn = log.GetCurrentTurn(); // gets turn.
    const Color color = piece.GetColor();    // gets color
    Square sq = piece.GetSquare();           // gets square.
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    game.RemovePiece(to_capture);
    game.AddPiece(to_capture, color, enemy_piece);
    game.RemovePiece(sq);

    Tier new_piece_tier = std::min(piece.GetTier() - 1, GetMaxTier(enemy_piece));
    SuperPieceInfo info = std::pair<enemy_piece, new_piece_tier>;
    game.ConvertToSuperPiece(info, to_capture);
}

std::unique_ptr<Ability> PawnConvertSuper::Clone() const
{
    return std::make_unique<PawnConvertSuper>(*this);
}