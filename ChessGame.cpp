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
        if( get_bit(board, i) )
        {
            U64 tempBoard = 0ULL;
            set_bit(tempBoard, i);
            if( PawnMask(tempBoard, true)   | PawnMask(tempBoard, false) )
                boardString += "P ";
            if( KnightMask(tempBoard, true) | KnightMask(tempBoard, false) )
                boardString += "N ";
            if( BishopMask(tempBoard, true) | BishopMask(tempBoard, false) )
                boardString += "B ";
            if( RookMask(tempBoard, true)   | RookMask(tempBoard, false) )
                boardString += "R ";
            if( QueenMask(tempBoard, true)  | QueenMask(tempBoard, false) )
                boardString += "Q ";
            if( KingMask(tempBoard, true)   | KingMask(tempBoard, false) )
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
}