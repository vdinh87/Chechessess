#pragma once
#include "KingSniperShot.hpp"

KingSniperShot::KingSniperShot(SuperChessGame* game_) :
Ability("King Sniper Shot", game_)
{
    
}

void KingSniperShot::Effect()
{
    
    std::string input_str;
    Square sq;
    std::cout << "Choose piece to delete: ";

    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else 
        std::cout << "Invalid square" << std::endl;
    
    game->RemovePiece(sq);
    std::cout << "KingSniperShot succeeded" << std::endl;
}

void KingSniperShot::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
}