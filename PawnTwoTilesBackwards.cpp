#pragma once
#include "PawnTwoTilesBackwards.hpp"

PawnTwoTilesBackwards::PawnTwoTilesBackwards(SuperChessGame& game_, Logger& log_) : 
Ability("Pawn Two Tiles Backwards", AbilityType::move_modifer, game_, log_)
{ }

void PawnTwoTilesBackwards::Modify(U64& move, Square piece)
{
    Color color = game.GetColor(1ULL << piece);
    U64 pawn = 1ULL << piece;
    U64 board = game.GetBoard();

    if (color == white && !(pawn & RANK_2) && !(South(pawn) & board)) //if white and not at rank 7.
        move |= pawn >> 16 & ~board;
    else if(color == black && !(pawn & RANK_7) && !(North(pawn) & board))
        move |= pawn << 16 & ~board;
}

std::unique_ptr<Ability> PawnTwoTilesBackwards::Clone() const
{
    return std::make_unique<PawnTwoTilesBackwards>(*this);
}