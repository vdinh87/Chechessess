#pragma once
#include "KingTurnIntoDead.hpp"

KingTurnIntoDead::KingTurnIntoDead(SuperChessGame& game_, Logger& log_) :
Ability("King Turn Into Dead", AbilityType::active, game_, log_, 5, 0)
{
}

bool KingTurnIntoDead::Effect(const SuperPiece& piece)
{
    Square sq = GetInputSquare("Choose piece to turn into dead: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }

    int current_turn = log.GetCurrentTurn();

    if( sq == piece.GetSquare() ){
        std::cout << "Cannot choose yourself\n";
        return false;
    }
    if ( !((1ULL << sq) & game.GetBoard()) ) { 
        std::cout << "Invalid square not a piece." << std::endl;
        return false;
    }
    if (piece.GetColor() != game.GetColor(1ULL << sq)) { 
        std::cout << "Invalid square different color piece." << std::endl;
        return false;
    } 
    if (GetCooldownTracker() < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return false;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return false;
    } 
    
    //ability time 
    std::vector<SuperPieceInfo> graveyard = game.GetPiecesInGraveyard( piece.GetColor() );

    if( graveyard.empty() )
    {
        std::cout << "Graveyard is empty.\n";
        return false;
    }

    int counter = 0;
    std::string output;
    for(const auto& p: graveyard)
    {
        std::string sp_str;
        if( p.second != Tier::not_superpiece )
            sp_str = TierStrings[p.second] + " Super";
        
        output += "(" + std::to_string(counter) + ") " + sp_str + PieceStrings[p.first] + " ";
        ++counter;
    }
    std::cout << output << std::endl;

    std::cout << "Select piece to resurrect: ";
    int selection;
    std::cin >> selection;
    std::cout << std::endl;

    SuperPieceInfo key;
    if( selection < graveyard.size() && selection >= 0 ) //valid number, not super strict checking
        key = graveyard[selection];    
    else{
        std::cout << "Not valid selection\n";
        return false;
    }

    game.RemovePiece(sq);
    game.RemoveFromGraveYard( std::make_pair(piece.GetColor(), key) );
    
    if( key.second != Tier::not_superpiece ) //if dead piece was superpiece
        game.AddSuperPiece(key, sq, piece.GetColor());
    else
        game.AddPiece(sq, piece.GetColor(), key.first);

    std::cout << "King Turn Into Dead successful" << std::endl;
    turn_last_used_ability = current_turn;
    return true;
}

std::unique_ptr<Ability> KingTurnIntoDead::Clone() const
{
    return std::make_unique<KingTurnIntoDead>(*this);
}