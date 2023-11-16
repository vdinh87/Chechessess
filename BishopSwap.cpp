#pragma once
#include "BishopSwap.hpp"

BishopSwap::BishopSwap(SuperChessGame& game_, Logger& log_) : 
Ability("Bishop Resurrect", AbilityType::active, game_, log_, 2, 2)
{
}

void BishopSwap::Effect(const SuperPiece &piece)
{
    Square sq = GetInputSquare("Choose Bishop to Swap ");
    if(sq == Square::invalid || game.GetPieceType(1ULL << sq) != Piece::Bishop)
    {
        std::cout << "Invalid square" << std::endl;
        return;
    }

    Color bishop_color = piece.GetColor();
    U64 bishop_spawn = (bishop_color == white) ? WhiteSpawnLocations[Bishop] : BlackSpawnLocations[Bishop];
    bishop_spawn = (bishop_color == white) ? (blackSquares & bishop_spawn) : (~blackSquares & bishop_spawn);

    int current_turn = log.GetCurrentTurn();

    if (bishop_spawn & game.GetBoard() == 0ULL){
        std::cout << "Opposite bishop Spawnlocation is currently inhabited by other piece\n";
        return;
    } if (GetCooldownTracker() < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return;
    } if (game.RemovePiece(sq) == true){
        turn_last_used_ability = 0;
        std::cout << "Bishopswap succeeded" << std::endl;
    }

    Square swapsquare = static_cast<Square>get_LSB(bishop_spawn);
    game.AddSuperPiece(piece.GetInfo(), swapsquare, piece.GetColor(), false);
    
    std::cout << "Bishop Swapped!! \n";
}

std::unique_ptr<Ability> BishopSwap::Clone() const
{
    return std::make_unique<BishopSwap>(*this);
}