#pragma once
#include "QueenKamikaze.hpp"

QueenKamikaze::QueenKamikaze(SuperChessGame& game_, Logger& log_) : 
Ability("Queen Kamikaze", AbilityType::on_capture, game_, log_)
{
    
}

bool QueenKamikaze::OnCapture(SuperPiece& piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    //captured square isn't the queen, ignore
    if( piece.GetSquare() != to_capture )
    {
        return false;
    }
    
    int current_turn = log.GetCurrentTurn();

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n";
        return false;
    }

    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn " << activation_turn << ".It's currently Turn [" << current_turn << "]\n";
        return false;
    }

    //effect
    U64 queen_pos = 1ULL << to_capture;
    U64 explode_list = 0ULL;
    for(int i = 0; i < 8; i++)
    {
        explode_list |= GoInDirection(static_cast<Direction>(i), queen_pos);
    }

    explode_list &= game.GetBoard() & ~game.GetBoardOf(piece.GetColor());

    to_explode = explode_list;
    //remove all pieces in explode radius
    while(explode_list != 0ULL)
    {
        int lsb = get_LSB(explode_list);
        game.RemovePiece( GetSquare(explode_list) );
        clear_bit(explode_list, lsb);
    }

    //remove queen
    keys_to_remove.push_back(to_capture);

    std::cout << "BOOM!" << std::endl;
    return true;
}

std::unique_ptr<Ability> QueenKamikaze::Clone() const
{
    return std::make_unique<QueenKamikaze>(*this);
}