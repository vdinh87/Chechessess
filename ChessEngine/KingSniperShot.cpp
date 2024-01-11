#pragma once
#include "KingSniperShot.hpp"

KingSniperShot::KingSniperShot(SuperChessGame &game_, Logger &log_) : 
Ability("King Sniper Shot", AbilityType::active, game_, log_, 10, 0)
{
}

bool KingSniperShot::Effect(const SuperPiece &piece)
{
    Square sq = GetInputSquare("Choose piece to delete to: ");
    if (sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }

    Color king_color = piece.GetColor();

    int current_turn = log.GetCurrentTurn();

    if (king_color == game.GetColor(1ULL << sq))
    {
        std::cout << "Invalid square Same color piece." << std::endl;
        return false;
    }

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n";
        return false;
    }

    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return false;
    }

    if (game.GetBoardOf(King, king_color) & (1ULL << sq))
    {
        std::cout << "You may not remove the King from the Board!!\n";
        return false;
    }

    if (game.RemovePiece(sq) == true)
    {
        turn_last_used_ability = current_turn;
        std::cout << "KingSniperShot succeeded" << std::endl;
        return true;
    }

    return false;
}

std::unique_ptr<Ability> KingSniperShot::Clone() const
{
    return std::make_unique<KingSniperShot>(*this);
}