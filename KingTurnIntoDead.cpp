#pragma once
#include "KingTurnIntoDead.hpp"

KingTurnIntoDead::KingTurnIntoDead(SuperChessGame* game_) :
Ability("King Sniper Shot", game_)
{
}

void KingTurnIntoDead::Effect(const SuperPiece& piece)
{
    std::string input_str;
    Square sq;
    std::cout << "Choose piece to turn into dead: ";

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

    if (piece.GetColor() != game->GetColor(1ULL << sq)) { 
        std::cout << "Invalid square different color piece." << std::endl;
        return;
    } if (cooldown_tracker < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n" ;
        return;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return;
    } 
    if (game->RemovePiece(sq) == true){
        cooldown_tracker = 0;
        std::cout << "KingTurnIntoDead succeeded" << std::endl;
    }
}

std::unique_ptr<Ability> KingTurnIntoDead::Clone() const
{
    return std::make_unique<KingTurnIntoDead>(*this);
}

void KingTurnIntoDead::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
    cooldown_tracker++;
}