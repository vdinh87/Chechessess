#pragma once
#include "KingTeleport.hpp"

KingTeleport::KingTeleport(SuperChessGame* game_) :
Ability("King Teleport", game_)
{
}

void KingTeleport::Effect(const SuperPiece& piece)
{
    std::string input_str; 
    Square sq;
    std::cout << "Choose piece to Teleport to: ";

    Color king_color = piece.GetColor();
    std::cin >> input_str; // input string
    std::cout << std::endl; // ends line created
    auto it = SqStrMap.find(input_str); // finds the piece to return square
    if (it != SqStrMap.end()) //if it finds it
        sq = it->second; //changes sq to that.
    else {
        std::cout << "Invalid square" << std::endl;
        return;
    }

    int current_turn = (log_.size() + 1) / 2; //current turn.

    if (game->GetBoard() & 1ULL << sq) 
        std::cout << "Cannot teleport into another piece." << std::endl;
    else if (cooldown_tracker < cooldown)
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n" ;
    else if (game->InCheck(king_color, piece.GetSquare()) || game->InCheck(king_color, sq))
        std::cout << "King In Check! Cannot Teleport!\n";
    else {
        game->Move(piece.GetSquare(), sq);
        std::cout << "Teleportation Succeeded _-=* mgic" << std::endl;
    }
    return;

}

std::unique_ptr<Ability> KingTeleport::Clone() const
{
    return std::make_unique<KingTeleport>(*this);
}

void KingTeleport::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
    cooldown_tracker++;
}