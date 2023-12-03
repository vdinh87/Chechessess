#pragma once
#include "PawnConvert.hpp"

PawnConvert::PawnConvert(SuperChessGame &game_, Logger &log_) : 
Ability("Pawn Convert", AbilityType::on_capture, game_, log_)
{
}

bool PawnConvert::OnCapture(SuperPiece &piece, Square to_capture, Square from_sq)
{
    Square sq = piece.GetSquare();        // gets square.
    if (from_sq != sq){
        std::cout <<"It's not your Piece\n";
        return false;
    }

    std::cout <<"Test: 1\n";
    const Color color = piece.GetColor(); // gets color
    Piece enemy_piece = game.GetPieceType(1ULL << to_capture);

    //virtual void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece) override

    std::cout <<"Our Current Square is " << piece.GetSquare();

    std::cout <<"Test: 2\n";
    if ( game.RemovePiece(to_capture) && game.AddPiece(to_capture, color, enemy_piece) && game.RemovePiece(sq)){
        std::cout <<"Returning True!\n";
        return true;
    }
    std::cout <<"Returning False!\n";

    return false;
    
}

std::unique_ptr<Ability> PawnConvert::Clone() const
{
    return std::make_unique<PawnConvert>(*this);
}
