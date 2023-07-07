#pragma once
#include "Knight.cpp"

#include <iostream>

class ChessGame
{
private:
    // white
    U64 WhitePawns;
    U64 WhiteKnights;
    U64 WhiteBishops;
    U64 WhiteRooks;
    U64 WhiteQueens;
    U64 WhiteKings;
    // black
    U64 BlackPawns;
    U64 BlackKnights;
    U64 BlackBishops;
    U64 BlackRooks;
    U64 BlackQueens;
    U64 BlackKings;
    //board
    U64 WhitePieces;
    U64 BlackPieces;
    U64 board;

    //masking functions
    bool PawnMask   (const U64& otherBoard, Color color) const;
    bool KnightMask (const U64& otherBoard, Color color) const;
    bool BishopMask (const U64& otherBoard, Color color) const;
    bool RookMask   (const U64& otherBoard, Color color) const;
    bool QueenMask  (const U64& otherBoard, Color color) const;
    bool KingMask   (const U64& otherBoard, Color color) const;
    void UpdateBoard();
public:
    ChessGame(/* args */);
    ~ChessGame();
    void PrintBoard() const;
};