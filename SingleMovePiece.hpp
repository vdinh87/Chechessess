#pragma once
#include <cstdint>
typedef uint64_t U64;

#define RANK_8 0xFF00000000000000ULL
#define RANK_1 0x00000000000000FFULL
#define FILE_A 0x0101010101010101ULL
#define FILE_H 0x8080808080808080ULL

class SingleMovePiece
{
private:
public:
    SingleMovePiece() = default;
    ~SingleMovePiece() = default;
    U64 North(U64 & other_board) const;
    U64 NorthWest(U64 & other_board) const;
    U64 NorthEast(U64 & other_board) const;
    U64 South(U64 & other_board) const;
    U64 SouthWest(U64 & other_board) const;
    U64 SouthEast(U64 & other_board) const;
};


