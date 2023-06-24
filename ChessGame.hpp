#pragma once
#include <cstdint>
#include <algorithm>
#include <string>
typedef uint64_t U64;

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))  
#define clear_bit(b, i) ((b) &= ~(1ULL << i)) 
#define get_LSB(b) (__builtin_ctzll(b))

enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// define sides
enum { white, black };

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
    bool PawnMask(const U64& otherBoard, bool color) const;
    bool KnightMask(const U64& otherBoard, bool color) const;
    bool BishopMask(const U64& otherBoard, bool color) const;
    bool RookMask(const U64& otherBoard, bool color) const;
    bool QueenMask(const U64& otherBoard, bool color) const;
    bool KingMask(const U64& otherBoard, bool color) const;
    void UpdateBoard();
public:
    ChessGame(/* args */);
    ~ChessGame();
    void PrintBoard() const;
};