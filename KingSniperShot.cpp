#pragma once
#include "KingSniperShot.hpp"

KingSniperShot::KingSniperShot(SuperChessGame* game_) :
Ability("King Sniper Shot", game_)
{
}

void KingSniperShot::Effect(const SuperPiece& piece)
{
    std::string input_str;
    Square sq;
    std::cout << "Choose piece to delete: ";
    
    Color king_color = piece.GetColor();
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else {
        std::cout << "Invalid square" << std::endl;
        return;
    }

    U64 king_board = game->GetBoardOf(King, king_color) & (1ULL << sq);


    int current_turn = (log_.size() + 1) / 2;
    
    if (king_color == game->GetColor(1ULL << sq)) { 
        std::cout << "Invalid square Same color piece." << std::endl;
        return;
    } if (cooldown_tracker < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n" ;
        return;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return;
    } if (game->GetBoardOf(King, king_color) & (1ULL << sq)) {
        std::cout << "You may not remove the King from the Board!!\n";
        return; 
    }if (game->RemovePiece(sq) == true){
        cooldown_tracker = 0;
        std::cout << "KingSniperShot succeeded" << std::endl;
    }
}

std::unique_ptr<Ability> KingSniperShot::Clone() const
{
    return std::make_unique<KingSniperShot>(*this);
}

void KingSniperShot::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
    cooldown_tracker++;
}