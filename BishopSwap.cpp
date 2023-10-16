#pragma once
#include "BishopSwap.hpp"

BishopSwap::BishopSwap(SuperChessGame *game_) : Ability("Bishop Resurrect", game_)
{
}

void BishopSwap::Effect(const SuperPiece &piece)
{
    std::string input_str;
    Square sq;
    std::cout << "Choose Bishop to Swap ";
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end() && game->GetPieceType(1ULL << it->second) == Bishop )
        sq = it->second;
    else {
        std::cout << "Invalid square" << std::endl;
        return;
    }
    
    Color bishop_color = piece.GetColor();
    U64 bishop_spawn = (bishop_color == white) ? WhiteSpawnLocations[Bishop] : BlackSpawnLocations[Bishop];
    bishop_spawn = (bishop_color == white) ? (blackSquares & bishop_spawn) : (~blackSquares & bishop_spawn);

    int current_turn = (log_.size() + 1) / 2;

    if (bishop_spawn & game->GetBoard() == 0ULL){
        std::cout << "Opposite bishop Spawnlocation is currently inhabited by other piece\n"
        return;
    } if (cooldown_tracker < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n" ;
        return;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return;
    } if (game->RemovePiece(sq) == true){
        cooldown_tracker = 0;
        std::cout << "KingSniperShot succeeded" << std::endl;
    }

    Square swapsquare = static_cast<Square>get_LSB(bishop_spawn);
    game->AddSuperPiece(piece.GetInfo(), swapsquare, piece.GetColor(), false);
    
    std::cout << "Bishop Swapped!! \n";

}

std::unique_ptr<Ability> BishopSwap::Clone() const
{
    return std::make_unique<BishopSwap>(*this);
}

void BishopSwap::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
    cooldown_tracker++;
}