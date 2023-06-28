#include "ChessGame.hpp"

ChessGame::ChessGame(/* args */)
{
    // white
    WhitePawns   = 0x000000000000FF00ULL;
    WhiteKnights = 0x0000000000000042ULL;
    WhiteBishops = 0x0000000000000024ULL;
    WhiteRooks   = 0x0000000000000081ULL;
    WhiteQueens  = 0x0000000000000008ULL;
    WhiteKings   = 0x0000000000000010ULL;
    // black
    BlackPawns   = 0x00FF000000000000ULL;
    BlackKnights = 0x4200000000000000ULL;
    BlackBishops = 0x2400000000000000ULL;
    BlackRooks   = 0x8100000000000000ULL;
    BlackQueens  = 0x0800000000000000ULL;
    BlackKings   = 0x1000000000000000ULL;
    UpdateBoard();
}

ChessGame::~ChessGame()
{
}

// Masking functions
// CHANGE TO USE ENUMS NOT BOOL
bool ChessGame::PawnMask(const U64& otherBoard, bool color) const
{
    //white
    if(color)
        return otherBoard & WhitePawns;
    //black
    return otherBoard & BlackPawns;
}

bool ChessGame::KnightMask(const U64& otherBoard, bool color) const
{ 
    //white
    if(color)
        return otherBoard & WhiteKnights;
    //black
    return otherBoard & BlackKnights;
}

bool ChessGame::BishopMask(const U64& otherBoard, bool color) const
{
    //white
    if(color)
        return otherBoard & WhiteBishops;
    //black
    return otherBoard & BlackBishops;
}

bool ChessGame::RookMask(const U64& otherBoard, bool color) const
{
    //white
    if(color)
        return otherBoard & WhiteRooks;
    //black
    return otherBoard & BlackRooks;
}

bool ChessGame::QueenMask(const U64& otherBoard, bool color) const
{
    //white
    if(color)
        return otherBoard & WhiteQueens;
    //black
    return otherBoard & BlackQueens;
}

bool ChessGame::KingMask(const U64& otherBoard, bool color) const
{
    //white
    if(color)
        return otherBoard & WhiteKings;
    //black
    return otherBoard & BlackKings;
}

void ChessGame::UpdateBoard()
{
    WhitePieces = WhitePawns | WhiteKnights | WhiteBishops | WhiteRooks | WhiteQueens | WhiteKings;
    BlackPieces = BlackPawns | BlackKnights | BlackBishops | BlackRooks | BlackQueens | BlackKings;
    board = WhitePieces | BlackPieces;
}

void ChessGame::PrintBoard() const
{
    std::string boardString;
    for( int i = 63; i >= 0; i-- ) 
    {
        U64 square = get_bit(board, i);
        if( square ) // contains piece  000000000000
        {
            if( PawnMask(square, true)   | PawnMask(square, false) )
                boardString += "P ";
            if( KnightMask(square, true) | KnightMask(square, false) )
                boardString += "N ";
            if( BishopMask(square, true) | BishopMask(square, false) )
                boardString += "B ";
            if( RookMask(square, true)   | RookMask(square, false) )
                boardString += "R ";
            if( QueenMask(square, true)  | QueenMask(square, false) )
                boardString += "Q ";
            if( KingMask(square, true)   | KingMask(square, false) )
                boardString += "K ";
        }
        else
            boardString += "0 ";
        
        // new line + reverse
        if( (i % 8) == 0 )
        {
            std::reverse(boardString.begin(), boardString.end());
            std::cout << boardString << std::endl;
            boardString.clear();
        }
    }   
}

int main()
{
    ChessGame cg;
    cg.PrintBoard();
    U64 bb = 1001ULL; 
    set_bit(bb,1); 
    std::cout << bb << std::endl;
}