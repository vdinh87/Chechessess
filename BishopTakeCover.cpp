#pragma once
#include "BishopTakeCover.hpp"

BishopTakeCover::BishopTakeCover(SuperChessGame *game_) : 
Ability("Bishop Resurrect", AbilityType::active, game_)
{
}

void BishopTakeCover::Effect(const SuperPiece &piece)
{
    int current_turn = (log_.size() + 1) / 2;

    const Color bishop_color = piece.GetColor();

    if (cooldown_tracker < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n";
        return;
    }
    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn 8. It's currently Turn [" << current_turn << "]\n";
        return;
    }
    
    std::vector<Direction> WhiteDirections = {SouthEast, SouthWest};
    std::vector<Direction> BlackDirections = {NorthEast, NorthWest};

    auto dirArr = piece.GetColor() == Color::white ? WhiteDirections : BlackDirections;
    for(int i = 0; i < dirArr.size(); i++)
        GetRay(piece.GetSquare(), )

}

std::unique_ptr<Ability> BishopTakeCover::Clone() const
{
    return std::make_unique<BishopTakeCover>(*this);
}

void BishopTakeCover::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
    cooldown_tracker++;
}