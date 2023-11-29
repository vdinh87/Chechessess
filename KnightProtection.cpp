#pragma once
#include "KnightProtection.hpp"

KnightProtection::KnightProtection(SuperChessGame& game_, Logger& log_) : 
Ability("Knight Protection", AbilityType::on_capture, game_, log_)
{
}

bool KnightProtection::OnCapture(SuperPiece& piece, Square to_capture)
{
    // one time protect already used
    if(!can_protect)
        return false;

    // capture piece not same color
    if(game.GetColor(to_capture) != piece.GetColor())
        return false;

    //effect
    U64 moves = game.GetAttacks( piece.GetSquare(), game.GetBoardOf(piece.GetColor()) & ~(1ULL << to_capture) );
    if( !((1ULL << to_capture) & moves) )
    {
        return false;
    }

    std::cout << "Knight protected!\n";
    can_protect = false;

    return true;
}


std::unique_ptr<Ability> KnightProtection::Clone() const
{
    return std::make_unique<KnightProtection>(*this);
}