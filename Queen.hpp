#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Queen : public Piece
{
private:

public:
    Queen(Square position_, Color color_);
    virtual ~Queen() = default;
    virtual U64 GetAttacks(U64 occupancy_) const; // pure virtual
};