#pragma once
#include "KingInspire.hpp"

KingInspire::KingInspire(SuperChessGame& game_, Logger& log_) : 
Ability("King Inspire", AbilityType::on_game_start, game_, log_)
{
}

void KingInspire::OnGameStart(SuperPiece& piece)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    int rand = dist(rng) * 5;
    Piece p_type = static_cast<Piece>(rand);
    std::vector<Square> squares = StartingSquares[ {piece.GetColor(), p_type} ];

    rand = dist(rng) * squares.size();
    Square rand_piece_sq = squares[rand];

    game.ConvertToSuperPiece( {p_type, Tier::T4 }, rand_piece_sq );
    std::cout << "King Inspire! Made " << SquareStrings[rand_piece_sq] << " into a superpiece!" << std::endl;
}

std::unique_ptr<Ability> KingInspire::Clone() const
{
    return std::make_unique<KingInspire>(*this);
}