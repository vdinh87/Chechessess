#pragma once
#include "BishopTakeCover.hpp"

BishopTakeCover::BishopTakeCover(SuperChessGame& game_, Logger& log_) : 
Ability("Bishop Take Cover", AbilityType::active, game_, log_)
{
}

bool BishopTakeCover::Effect(const SuperPiece &piece)
{
    const Color bishop_color = piece.GetColor();

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n";
        return false;
    }
    if (log.GetCurrentTurn() < activation_turn)
    {
        std::cout << name << " is only Available at turn 8. It's currently Turn [" << log.GetCurrentTurn() << "]\n";
        return false;
    }
    
    U64 moves = 0ULL;
    std::vector<Direction> WhiteDirections = {Direction::SE, Direction::SW};
    std::vector<Direction> BlackDirections = {Direction::NE, Direction::NW};

    auto dirArr = piece.GetColor() == Color::white ? WhiteDirections : BlackDirections;
    for(int i = 0; i < dirArr.size(); i++)
    {
        U64 collision = GetFirstCollision(piece.GetSquare(), dirArr[i], game.GetBoard());
        if( collision == 0ULL )
            continue;
        if( game.GetColor(collision) != bishop_color )
            continue;
        else if( !(GoInDirection(dirArr[i], collision) & game.GetBoard()) )
            moves |= GoInDirection(dirArr[i], collision);
    }
    
    if(moves == 0ULL)
    {
        std::cout << "No possible moves" << std::endl;
        return false;
    }
    PrintGoard(moves);

    Square sq = GetInputSquare("Choose square to move to: ");
    if(sq == Square::invalid)
    {
        std::cout << "Invalid square" << std::endl;
        return false;
    }
    if( !((1ULL << sq) & moves) )
    {
        std::cout << "Square not in moveset" << std::endl;
        return false;
    }

    game.Move(piece.GetSquare(), sq);
    std::cout << "Bishop Took Cover Woahhhh" << std::endl;

    return true;
}

std::unique_ptr<Ability> BishopTakeCover::Clone() const
{
    return std::make_unique<BishopTakeCover>(*this);
}