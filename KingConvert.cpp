#pragma once
#include "KingConvert.hpp"

KingConvert::KingConvert(SuperChessGame& game_, Logger& log_) :
Ability("King Convert", AbilityType::active, game_, log_, 10, 15)
{
}

void KingConvert::Effect(const SuperPiece& piece)
{
    Square sq = GetInputSquare("Choose piece to convert: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return;
    }

    int current_turn = log.GetCurrentTurn();

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return;
    } 

    if ( current_turn < activation_turn )
    {
        std::cout << name << " is only Available at turn 15. It's currently Turn [" << current_turn << "]\n";
        return;
    } 

    if ( (game.ConvertPieceToSide(sq, piece.GetColor()) &&
            game.ConvertToSuperPiece(std::make_pair(game.GetPieceType(1ULL << sq), piece.GetInfo().second), sq)) || 
            (game.ConvertPieceToSide(sq, piece.GetColor()) &&
            game.UpgradeSuperPieceTier(sq, piece.GetInfo().second)) ) 
    {
        turn_last_used_ability = 0;
        std::cout << "KingConvert succeeded" << std::endl;
    }
}

std::unique_ptr<Ability> KingConvert::Clone() const
{
    return std::make_unique<KingConvert>(*this);
}