#pragma once
#include "Piece.hpp"
#define RANK_7 0x00FF000000000000ULL
#define RANK_1 0x00000000000000FFULL


class Pawn : public Piece
{
private:
bool has_moved = false;
public:
 Pawn(Square position_);
 virtual ~Pawn();
 virtual U64 GetAttacks(U64 occupancy_) const;
};
