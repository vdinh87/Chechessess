#include "SingleMovePiece.hpp"

U64 SingleMovePiece::North(U64 & other_board) const
{
    if( ~(other_board & RANK_8) )
        return other_board << 8;
    return 0ULL;
}

U64 SingleMovePiece::NorthWest(U64 & other_board) const
{
    if( ~(other_board & RANK_8) | ~(other_board & FILE_A) )
        return other_board << 7;
    return 0ULL;
}

U64 SingleMovePiece::NorthEast(U64 & other_board) const
{
    if( ~(other_board & RANK_8) | ~(other_board & FILE_H) )
        return other_board << 9;
    return 0ULL;
}

U64 SingleMovePiece::South(U64 & other_board) const
{
    if( ~(other_board & RANK_1) )
        return other_board >> 8;
    return 0ULL;
}

U64 SingleMovePiece::SouthWest(U64 & other_board) const
{
    if( ~(other_board & RANK_1) | ~(other_board & FILE_A) )
        return other_board >> 9;
    return 0ULL;
}

U64 SingleMovePiece::SouthEast(U64 & other_board) const
{
    if( ~(other_board & RANK_1) | ~(other_board & FILE_H) )
        return other_board >> 7;
    return 0ULL;
}

U64 SingleMovePiece::East(U64 & other_board) const
{
    if( ~(other_board & FILE_H) )
        return other_board >> 7;
    return 0ULL;
}

