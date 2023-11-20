#pragma once
#include "PawnConvertSuper.hpp"

PawnConvertSuper::PawnConvertSuper(SuperChessGame &game_, Logger &log_) : Ability("Pawn Convert Piece", AbilityType::active, game_, log_, 0, 0)
{
}

void PawnConvertSuper::OnCapture(SuperPiece &piece, SuperPiece &other_piece)
{
    int current_turn = log.GetCurrentTurn();
    const Color color = piece.GetColor();
    Square sq = piece.GetSquare();
    Square enemy_sq = other_piece.GetSquare();

    U64 &our_pawn_board = game.GetBoardOf(Pawn, color);
    U64 &enemy_board = game.GetBoardOf(game.GetPieceType(other_piece.GetSquare()), !color);
    U64 &our_newpiece_board = game.GetBoardOf(game.GetPieceType(other_piece.GetSquare()), color);

    clear_bit(enemy_board, enemy_sq);
    set_bit(our_newpiece_board, enemy_sq);
    clear_bit(our_pawn_board, sq);

    SuperPieceInfo info = std::pair<other_piece.GetInfo().first, piece.GetInfo().second>;
    game.ConvertToSuperPiece(info, enemy_sq);
}

std::unique_ptr<Ability> PawnConvertSuper::Clone() const
{
    return std::make_unique<PawnConvertSuper>(*this);
}

void PawnConvertSuper::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
}