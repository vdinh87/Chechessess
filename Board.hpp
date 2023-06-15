#pragma once
#include <bitset>


class Board
{
private:
    std::bitset<64> WhitePawns;
    std::bitset<64> BlackPawns;
    std::bitset<64> WhiteKnights;
    std::bitset<64> BlackKnights;
    std::bitset<64> WhiteBishops;
    std::bitset<64> BlackBishops;
    std::bitset<64> WhiteRooks;
    std::bitset<64> BlackRooks;
    std::bitset<64> WhiteQueens;
    std::bitset<64> BlackQueens;
    std::bitset<64> WhiteKing;
    std::bitset<64> BlackKing;
    
public:
    Board(/* args */);
    ~Board();
};