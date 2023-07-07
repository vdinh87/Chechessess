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
bool ChessGame::PawnMask(const U64& otherBoard, Color color) const
{
    //white
    if(color)
        return otherBoard & WhitePawns;
    //black
    return otherBoard & BlackPawns;
}

bool ChessGame::KnightMask(const U64& otherBoard, Color color) const
{ 
    //white
    if(color)
        return otherBoard & WhiteKnights;
    //black
    return otherBoard & BlackKnights;
}

bool ChessGame::BishopMask(const U64& otherBoard, Color color) const
{
    //white
    if(color)
        return otherBoard & WhiteBishops;
    //black
    return otherBoard & BlackBishops;
}

bool ChessGame::RookMask(const U64& otherBoard, Color color) const
{
    //white
    if(color)
        return otherBoard & WhiteRooks;
    //black
    return otherBoard & BlackRooks;
}

bool ChessGame::QueenMask(const U64& otherBoard, Color color) const
{
    //white
    if(color)
        return otherBoard & WhiteQueens;
    //black
    return otherBoard & BlackQueens;
}

bool ChessGame::KingMask(const U64& otherBoard, Color color) const
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
    U64 square;
    for( int i = 63; i >= 0; i-- ) 
    {
        square = get_bit(board, i);
        if( square )
        {
            if( PawnMask(square, white)   | PawnMask(square, black) )
                boardString += "P ";
            if( KnightMask(square, white) | KnightMask(square, black) )
                boardString += "N ";
            if( BishopMask(square, white) | BishopMask(square, black) )
                boardString += "B ";
            if( RookMask(square, white)   | RookMask(square, black) )
                boardString += "R ";
            if( QueenMask(square, white)  | QueenMask(square, black) )
                boardString += "Q ";
            if( KingMask(square, white)   | KingMask(square, black) )
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

void PrintBoard(U64 board)
{
    std::string boardString;
    U64 square;
    for( int i = 63; i >= 0; i-- ) 
    {
        square = get_bit(board, i);
        if( square )
            boardString += "1 ";
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
    U64 bb = 1ULL;
    PrintBoard(bb);
    std::cout << std::endl;
    ChessGame cg;
    cg.PrintBoard();

    SlidingPiece sp;
}