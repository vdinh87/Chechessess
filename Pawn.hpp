#pragma once
#include "Piece.hpp"
#include "SingleMovePiece.cpp"
#define RANK_7 0x00FF000000000000ULL
#define RANK_2 0x000000000000FF00ULL


class Pawn : public Piece, public SingleMovePiece
{
private:
    bool has_moved = false;
public:
    Pawn(Square position_, Color color_);
    virtual ~Pawn() = default;
    virtual U64 GetAttacks(U64 occupancy_) const;
    virtual void SetPosition(Square new_position);
};
