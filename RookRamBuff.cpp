#pragma once
#include "RookRamBuff.hpp"

RookRamBuff::RookRamBuff(SuperChessGame& game_, Logger& log_) : 
Ability("Bishop Resurrect", AbilityType::active, game_, log_, 6, 0)
{
}

void RookRamBuff::OnCapture(SuperPiece& piece, SuperPiece& other_piece)
{
    int current_turn = log.GetCurrentTurn();

    const Color bishop_color = piece.GetColor();

    if (cooldown_tracker < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n";
        return;
    }

    //effect
    if( !HasBuff(piece.GetSquare()) )
    {
        std::cout << "No buff" << std::endl;
        return;
    }
}

bool RookRamBuff::HasBuff(SuperPiece& piece) const
{
    if(log_.size() < 2) // no prev move
        return false;

    //Check if prev move was by this rook and it captured
    ChessMove move = log_[log_.size()-2];
    if ( !(move.type == Piece::Rook && move.actions == Action::Capture && move.to == piece.GetSquare()) )
        return false;

    //Check if rook is in line with other rook
    std::vector<Direction> dir_arr = { Direction::N, Direction::E, Direction::S, Direction::W };
    for( const auto d: dir_arr )
    {
        if( GetFirstCollision(piece.GetSquare(), d, game->GetBoard()) & game->GetBoardOf(Piece::Rook, piece.GetColor()) )
            return true;
    }
    return false;
}

std::unique_ptr<Ability> RookRamBuff::Clone() const
{
    return std::make_unique<RookRamBuff>(*this);
}

void RookRamBuff::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
    cooldown_tracker++;
}