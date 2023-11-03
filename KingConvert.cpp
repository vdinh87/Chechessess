#pragma once
#include "KingConvert.hpp"

KingConvert::KingConvert(SuperChessGame* game_) :
Ability("King Convert", AbilityType::active, game_)
{
}

void KingConvert::Effect(const SuperPiece& piece)
{
    std::string input_str;
    Square sq;
    std::cout << "Choose piece to convert: ";
    
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else {
        std::cout << "Invalid square" << std::endl;
        return;
    }

    int current_turn = (log_.size() + 1) / 2;

    if (cooldown_tracker < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n" ;
        return;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 15. It's currently Turn [" << current_turn << "]\n";
        return;
    } if ( (game->ConvertPieceToSide(sq, piece.GetColor()) &&
            game->ConvertToSuperPiece(std::make_pair(game->GetPieceType(1ULL << sq), piece.GetInfo().second), sq)) || 
            (game->ConvertPieceToSide(sq, piece.GetColor()) &&
            game->UpgradeSuperPieceTier(sq, piece.GetInfo().second)) ) {
        cooldown_tracker = 0;
        std::cout << "KingConvert succeeded" << std::endl;
    }
}

std::unique_ptr<Ability> KingConvert::Clone() const
{
    return std::make_unique<KingConvert>(*this);
}

void KingConvert::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
    cooldown_tracker++;
}