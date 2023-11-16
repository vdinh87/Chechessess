#pragma once
#include "KingTeleport.hpp"

KingTeleport::KingTeleport(SuperChessGame& game_, Logger& log_) :
Ability("King Teleport", AbilityType::active, game_, log_, 5, 0)
{
}

void KingTeleport::Effect(const SuperPiece& piece)
{
    Square sq = GetInputSquare("Choose square to Teleport to: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return;
    }
    
    Color king_color = piece.GetColor();

    int current_turn = log.GetCurrentTurn(); //current turn.

    if (game.GetBoard() & 1ULL << sq) 
        std::cout << "Cannot teleport into another piece." << std::endl;
    else if (GetCooldownTracker() < cooldown)
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
    else if (game.InCheck(king_color) || game.InCheck(king_color))
        std::cout << "King Will Be In Check! Cannot Teleport!\n";
    else {
        game.Move(piece.GetSquare(), sq);
        std::cout << "Teleportation Succeeded _-=* mgic" << std::endl;
    }
    
    return;

}

std::unique_ptr<Ability> KingTeleport::Clone() const
{
    return std::make_unique<KingTeleport>(*this);
}