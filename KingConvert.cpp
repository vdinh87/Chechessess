#pragma once
#include "KingConvert.hpp"

KingConvert::KingConvert(SuperChessGame& game_, Logger& log_) :
Ability("King Convert", AbilityType::active, game_, log_, 10, 15)
{
}

bool KingConvert::Effect(const SuperPiece& piece)
{
    Square sq = GetInputSquare("Choose piece to convert: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }

    int current_turn = log.GetCurrentTurn();

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return false;
    } 

    if ( current_turn < activation_turn )
    {
        std::cout << name << " is only Available at turn 15. It's currently Turn [" << current_turn << "]\n";
        return false;
    } 

    if ( game.ConvertPieceToSide(sq, piece.GetColor()) &&
        (game.ConvertToSuperPiece( {game.GetPieceType(1ULL << sq), piece.GetInfo().second} , sq) || 
         game.UpgradeSuperPieceTier(sq, piece.GetInfo().second)) ) 
    {
        turn_last_used_ability = current_turn;
        std::cout << "KingConvert succeeded" << std::endl;
        return true;
    }
    return false;
}

std::unique_ptr<Ability> KingConvert::Clone() const
{
    return std::make_unique<KingConvert>(*this);
}