#pragma once
#include "KingTeleport.hpp"

KingTeleport::KingTeleport(SuperChessGame& game_, Logger& log_) :
Ability("King Teleport", AbilityType::active, game_, log_, 5, 0)
{
}

bool KingTeleport::Effect(const SuperPiece& piece)
{
    Square sq = GetInputSquare("Choose square to Teleport to: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }
    
    Color king_color = piece.GetColor();

    int current_turn = log.GetCurrentTurn(); //current turn.

    if (game.GetBoard() & 1ULL << sq)
    {
        std::cout << "Cannot teleport into another piece." << std::endl;
        return false;
    }
    else if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return false;
    }
    else if (game.InCheck(king_color) || game.InCheck(king_color))
    {
        std::cout << "King Will Be In Check! Cannot Teleport!\n";
        return false;
    }
    else {
        game.Move(piece.GetSquare(), sq);
        turn_last_used_ability = 0;
        std::cout << "Teleportation Succeeded _-=* mgic" << std::endl;
        return true;
    }
    
    return false;
}

std::unique_ptr<Ability> KingTeleport::Clone() const
{
    return std::make_unique<KingTeleport>(*this);
}