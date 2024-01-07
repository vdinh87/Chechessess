#pragma once
#include "PawnConvert.hpp"

PawnConvert::PawnConvert(SuperChessGame &game_, Logger &log_) : 
Ability("Pawn Convert", AbilityType::on_capture, game_, log_)
{
}

bool PawnConvert::OnCapture(SuperPiece &piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    Square sq = piece.GetSquare();
    if (from_sq != sq){
        return false;
    }

    const Color color = piece.GetColor(); // gets color
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    if ( game.RemovePiece(to_capture) && game.AddPiece(to_capture, color, enemy_piece)){
        keys_to_remove.push_back(sq);
        return true;
    }

    return false;
}

std::unique_ptr<Ability> PawnConvert::Clone() const
{
    return std::make_unique<PawnConvert>(*this);
}
