#pragma once
#include "RookRamBuff.hpp"

RookRamBuff::RookRamBuff(SuperChessGame& game_, Logger& log_) : 
Ability("Rook Ram Buff", AbilityType::on_capture, game_, log_, 6, 0)
{
}

bool RookRamBuff::OnCapture(SuperPiece& piece, Square to_capture, Square from_sq)
{
    //captured square isn't the rook, ignore
    if( piece.GetSquare() != to_capture )
        return false;
    int current_turn = log.GetCurrentTurn();

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n";
        return false;
    }

    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn " << activation_turn << ".It's currently Turn [" << current_turn << "]\n";
        return false;
    }

    //effect
    if( !HasBuff(piece) )
    {
        return false;
    }

    // return rook to last position 
    auto l = log.ReadLog(); 
    ChessMove move = l[log.GetSize()-1];

    game.ExecuteMove( piece.GetColor(), piece.GetSquare(), move.from, piece.GetPieceType(), piece.GetPieceType() );
    return true;
}

bool RookRamBuff::HasBuff(SuperPiece& piece) const
{
    if(log.GetSize() < 2) // no prev move
        return false;

    //Check if prev move was by this rook and it captured
    auto l = log.ReadLog();
    ChessMove move = l[log.GetSize()-1];
    
    bool captured_last_turn = false;
    for(const auto& a: move.actions)
    {
        if(a == Action::Capture)
        {
            captured_last_turn = true;
            break;
        }
    }

    if ( move.type != Piece::Rook || !captured_last_turn || move.to != piece.GetSquare() ) 
    {
        return false;
    }

    //Check if rook is in line with other rook
    std::vector<Direction> dir_arr = { Direction::N, Direction::E, Direction::S, Direction::W };
    for( const auto d: dir_arr )
    {
        if( GetFirstCollision(piece.GetSquare(), d, game.GetBoard()) & game.GetBoardOf(Piece::Rook, piece.GetColor()) )
        {
            return true;
        }
    }

    return false;
}

std::unique_ptr<Ability> RookRamBuff::Clone() const
{
    return std::make_unique<RookRamBuff>(*this);
}