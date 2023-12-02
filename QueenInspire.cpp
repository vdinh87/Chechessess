#pragma once
#include "QueenInspire.hpp"

QueenInspire::QueenInspire(SuperChessGame& game_, Logger& log_) : 
Ability("Queen Inspire", AbilityType::on_game_start, game_, log_)
{
}

QueenInspire::~QueenInspire()
{
    //strip knights of their super abilities
    U64 knights = game.GetBoardOf(Piece::Knight, color);
    while(knights != 0ULL)
    {
        int lsb = get_LSB(knights);
        game.UnSuper( GetSquare(knights) );
        clear_bit(knights, lsb);
    }
    std::cout << "QUEEN INSPIRE\n";
}

void QueenInspire::OnGameStart(SuperPiece& piece)
{
    color = piece.GetColor();
    game.ConvertSuperPiecesofType( {Piece::Knight, Tier::T0 }, color );
}

std::unique_ptr<Ability> QueenInspire::Clone() const
{
    return std::make_unique<QueenInspire>(*this);
}