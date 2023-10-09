#pragma once
#include "SuperChessGame.hpp"
#include "AbilityLibrary.cpp"
#include "SuperPiece.cpp"
SuperChessGame::SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // InitSuperPieces(white, black);
    al = std::make_shared<AbilityLibrary>(this);

    // AddSuperPiece(white_info, Square::b3, Color::white);
    ConvertToSuperPiece(white_info, Square::e1);
}

bool SuperChessGame::AddSuperPiece(SuperPieceInfo info, Square square, Color color, bool conversion)
{   
    if( !ChessGame::AddPiece(square, color, info.first) )
        return false;
    if( conversion && !(board & (1ULL << square)) ) //square doesn't contain piece to convert
        return false;
    if ( !conversion && (board & (1ULL << square)) ) //regular add, checks if not piece there
        return false;
    
    std::vector<std::unique_ptr<Ability>> v;
    v.push_back(al->GetAbility(info));
    super_pieces[square] = std::make_shared<SuperPiece>(v, info, square, color);

    return true;
}

bool SuperChessGame::AddSuperPiecesofType(SuperPieceInfo info, Color color)
{
    return false;
}


bool SuperChessGame::RemovePiece(Square square)
{
    if (!ChessGame::RemovePiece(square))
        return false;
    
    //remove if superpiece
    if( IsSuperPiece(square) )
        super_pieces.erase(square);
        
    UpdateBoard();
    return true;
}

bool SuperChessGame::ConvertToSuperPiece(SuperPieceInfo info, Square square)
{
    auto it = super_pieces.find(square);
    if( it != super_pieces.end() )
        return false;
    return AddSuperPiece(info, square, GetColor(1ULL << square), true);
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    // for passive abilities later
    return ChessGame::Move(from_sq, to_sq);
}

void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    ChessGame::ExecuteMove(color, from_sq, to_sq, from_piece, to_piece);

    //remove to_piece
    if( IsSuperPiece(to_sq) )
        super_pieces.erase(to_sq);
    //move from_piece
    if( IsSuperPiece(from_sq) )
    {
        super_pieces.erase(to_sq);
        super_pieces[to_sq] = std::move(super_pieces[from_sq]);
        super_pieces.erase(from_sq);
        super_pieces[to_sq]->UpdateSquare(to_sq);
    }
}
// init
void SuperChessGame::InitSuperPieces(const SuperPieceInfo &white, const SuperPieceInfo &black)
{
}

//utility
bool SuperChessGame::IsSuperPiece(const Square& key) const
{
    for(auto& e: super_pieces)
        std::cout << "SP";
    auto it = super_pieces.find(key);
    if( it != super_pieces.end() )
        return true;

    return false;
}

// misc
void SuperChessGame::Do(Square sq, Tier t)
{
    super_pieces[sq]->UseAbility(t);
}