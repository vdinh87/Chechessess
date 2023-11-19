#pragma once
#include "BishopSwap.hpp"

BishopSwap::BishopSwap(SuperChessGame& game_, Logger& log_) : 
Ability("Bishop Swap", AbilityType::active, game_, log_, 2, 2)
{
}

bool BishopSwap::Effect(const SuperPiece &piece)
{
    Square sq = GetInputSquare("Choose Bishop to Swap ");
    if(sq == Square::invalid || game.GetPieceType(1ULL << sq) != Piece::Bishop)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }

    Color bishop_color = piece.GetColor();
    U64 bishop_spawn = (bishop_color == white) ? WhiteSpawnLocations[Bishop] : BlackSpawnLocations[Bishop];
    bishop_spawn = (bishop_color == white) ? (blackSquares & bishop_spawn) : (~blackSquares & bishop_spawn);

    int current_turn = log.GetCurrentTurn();

    if (bishop_spawn & game.GetBoard() == 0ULL){
        std::cout << "Opposite bishop Spawnlocation is currently inhabited by other piece\n";
        return false;
    } if (GetCooldownTracker() < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return false;
    } if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return false;
    } if (game.RemovePiece(sq) == true){
        turn_last_used_ability = current_turn;
    }

    Square swapsquare = static_cast<Square>get_LSB(bishop_spawn);
    game.AddSuperPiece(piece.GetInfo(), swapsquare, piece.GetColor(), false);
    
    std::cout << "Bishop Swapped!! \n";
    return true;
}

std::unique_ptr<Ability> BishopSwap::Clone() const
{
    return std::make_unique<BishopSwap>(*this);
}