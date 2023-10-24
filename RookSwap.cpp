#pragma once
#include "RookSwap.hpp"

RookSwap::RookSwap(SuperChessGame *game_) : Ability("Bishop Resurrect", game_)
{
}

void RookSwap::Effect(const SuperPiece &piece)
{
    Color rook_color = piece.GetColor();
    std::string input_str;
    Square sq;
    std::cout << "Choose Rook to Swap ";
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end() && game->GetPieceType(1ULL << it->second) == Rook && game->GetColor(1ULL << it->second) == rook_color )
        sq = it->second;
    else {
        std::cout << "Invalid square" << std::endl;
        return;
    }
    


    U64 king_board = game->GetBoardOf(King, rook_color);

    int current_turn = (log_.size() + 1) / 2;

    U64 rook_rows_and_cols = (0xFFULL << ((sq / 8) * 8)) | (0x0101010101010101ULL << (sq % 8));

    
    if ( !game->IsSuperPiece(sq) ){
        std::cout << "Rook Selected is not a superpiece, cannot swap it.\n"
        return;
    } if ( king_board & rook_rows_and_cols == 0ULL ){
        std::cout << "King is not in the same Row or Column as the rook.\n"
        return;
    } if (game->InCheck(rook_color)){
        std::cout << "King Currently in Check! Cannot swap.\n"
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
    Square swapsquare = static_cast<Square>get_LSB(king_board); // king qsuare
    
    game->RemovePiece(swapsquare);
    SuperPieceInfo info = piece.GetInfo(); // need to get it before i delete it.
    game->RemovePiece(sq);
    game->AddSuperPiece(info, swapsquare, rook_color, false);
    
    if (game->IsSuperPiece(swapsquare)){
        SuperPieceInfo info = std::make_pair(Piece::King, Tier::T0); //TODO: Temporarily t0 since i can't get tier from piece im not accessing
        game->AddSuperPiece( info , sq, rook_color, false)
    } else {
        game->AddPiece(sq, rook_color, King);
    }



    std::cout << "Rook Swapped!! \n";

}

std::unique_ptr<Ability> RookSwap::Clone() const
{
    return std::make_unique<RookSwap>(*this);
}

void RookSwap::Notify(const std::vector<ChessMove> &log)
{
    log_ = log;
    cooldown_tracker++;
}