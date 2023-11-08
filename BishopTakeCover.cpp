#pragma once
#include "BishopTakeCover.hpp"

BishopTakeCover::BishopTakeCover(SuperChessGame *game_) : 
Ability("Bishop Resurrect", AbilityType::active, game_)
{
}

void BishopTakeCover::Effect(const SuperPiece &piece)
{
    int current_turn = (log_.size() + 1) / 2;

    const Color bishop_color = piece.GetColor();

    if (cooldown_tracker < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - cooldown_tracker << "\n";
        return;
    }
    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn 8. It's currently Turn [" << current_turn << "]\n";
        return;
    }
    
    U64 moves = 0ULL;
    std::vector<Direction> WhiteDirections = {Direction::SE, Direction::SW};
    std::vector<Direction> BlackDirections = {Direction::NE, Direction::NW};

    auto dirArr = piece.GetColor() == Color::white ? WhiteDirections : BlackDirections;
    for(int i = 0; i < dirArr.size(); i++)
    {
        U64 collision = GetFirstCollision(piece.GetSquare(), dirArr[i], game->GetBoard());
        if( collision == 0ULL )
            continue;
        if( game->GetColor(collision) != bishop_color )
            continue;
        else if( !(GoInDirection(dirArr[i], collision) & game->GetBoard()) )
            moves |= GoInDirection(dirArr[i], collision);
    }
    
    if(moves == 0ULL)
    {
        std::cout << "No possible moves" << std::endl;
        return;
    }
    PrintGoard(moves);

    Square sq = GetInputSquare();
    if(sq == Square::invalid)
        return;

    if( !((1ULL << sq) & moves) )
    {
        std::cout << "Square not in moveset" << std::endl;
        return;
    }

    game->Move(piece.GetSquare(), sq);
    std::cout << "Bishop Took Cover Woahhhh" << std::endl;

    // return true;
}

std::unique_ptr<Ability> BishopTakeCover::Clone() const
{
    return std::make_unique<BishopTakeCover>(*this);
}

void BishopTakeCover::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
    cooldown_tracker++;
}