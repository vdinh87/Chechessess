#pragma once
#include "RookSwap.hpp"

RookSwap::RookSwap(SuperChessGame& game_, Logger& log_) : 
Ability("Rook Swap", AbilityType::active,  game_, log_, 2, 2)
{
}

bool RookSwap::Effect(const SuperPiece &piece)
{
    Color rook_color = piece.GetColor();

    Square sq = piece.GetSquare();

    U64 king_board = game.GetBoardOf(King, rook_color);

    int current_turn = log.GetCurrentTurn();

    U64 rook_rows_and_cols = (0xFFULL << ((sq / 8) * 8)) | (0x0101010101010101ULL << (sq % 8));

    if ( !game.IsSuperPiece(sq) ){
        std::cout << "Rook Selected is not a superpiece, cannot swap it.\n";
        return false;
    } 
    if ( king_board & rook_rows_and_cols == 0ULL ){
        std::cout << "King is not in the same Row or Column as the rook.\n";
        return false;
    } 
    if (game.InCheck(rook_color)){
        std::cout << "King Currently in Check! Cannot swap.\n";
        return false;
    } 
    if (GetCooldownTracker() < cooldown){
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n" ;
        return false;
    } 
    if ( current_turn < activation_turn ){
        std::cout << name << " is only Available at turn 10. It's currently Turn [" << current_turn << "]\n";
        return false;
    } 
    if (game.RemovePiece(sq) == true){
        turn_last_used_ability = current_turn;
    }

    Square swapsquare = static_cast<Square>get_LSB(king_board); // king qsuare
    
    game.Swap(sq ,swapsquare);
    std::cout << "Rook Swapped!! \n";
    return true;
}

std::unique_ptr<Ability> RookSwap::Clone() const
{
    return std::make_unique<RookSwap>(*this);
}