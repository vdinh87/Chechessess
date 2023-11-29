#pragma once
#include "PawnConvert.hpp"

PawnConvert::PawnConvert(SuperChessGame &game_, Logger &log_) : Ability("Pawn Convert Piece", AbilityType::active, game_, log_)
{
}

bool PawnConvert::OnCapture(SuperPiece &piece, Square to_capture)
{
    const Color color = piece.GetColor(); // gets color
    Square sq = piece.GetSquare();        // gets square.
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    game.RemovePiece(to_capture);
    game.AddPiece(to_capture, color, enemy_piece);
    game.RemovePiece(sq);
}

std::unique_ptr<Ability> PawnConvert::Clone() const
{
    return std::make_unique<PawnConvert>(*this);
}
