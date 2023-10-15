#pragma once
#include "KingTurnIntoDead.hpp"

KingTurnIntoDead::KingTurnIntoDead(SuperChessGame* game_) :
Ability("King Turn Into Dead", game_)
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

    if( sq == piece.GetSquare() ){
        std::cout << "Cannot choose yourself\n";
        return;
    }
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
    
    //ability time 
    std::vector<SuperPieceInfo> graveyard = game->GetPiecesInGraveyard( piece.GetColor() );

    if( graveyard.empty() )
    {
        std::cout << "Graveyard is empty.\n";
        return;
    }

    int counter = 0;
    std::string output;
    for(const auto& p: graveyard)
    {
        std::string sp_str;
        if( p.second != Tier::not_superpiece )
            sp_str = TierStrings[p.second] + " Super";
        
        output += "(" + std::to_string(counter) + ") " + sp_str + PieceStrings[piece.GetInfo().first] + " ";
        ++counter;
    }

    std::cout << "Select piece to resurrect: ";
    int selection;
    std::cin >> selection;
    std::cout << std::endl;

    SuperPieceInfo key;
    if( selection < graveyard.size() && selection >= 0 ) //valid number, not super strict checking
        key = graveyard[selection];    
    else{
        std::cout << "Not valid selection\n";
        return;
    }
    game->RemovePiece(sq);
    if( key.second != Tier::not_superpiece ) //if dead piece was superpiece
        game->AddSuperPiece(key, sq, piece.GetColor());
    else
        game->AddPiece(sq, piece.GetColor(), key.first);

    std::cout << "King Turn Into Dead successful";
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