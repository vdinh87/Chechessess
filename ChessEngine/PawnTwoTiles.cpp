#pragma once
#include "PawnTwoTiles.hpp"

PawnTwoTiles::PawnTwoTiles(SuperChessGame& game_, Logger& log_) : 
Ability("Pawn Two Tiles", AbilityType::move_modifer, game_, log_)
{

}

void PawnTwoTiles::Modify(U64& move, Square piece)
{
    Color color = game.GetColor(1ULL << piece);
    U64 pawn = 1ULL << piece;
    U64 board = game.GetBoard();

    if (color == white && !(pawn & RANK_7) && !(North(pawn) & board)) //if white and not at rank 7.
        move |= pawn << 16 & ~board;
    else if(color == black && !(pawn & RANK_2) && !(South(pawn) & board))
        move |= pawn >> 16 & ~board;
}

std::unique_ptr<Ability> PawnTwoTiles::Clone() const
{
    return std::make_unique<PawnTwoTiles>(*this);
}