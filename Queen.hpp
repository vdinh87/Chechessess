#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Queen : public Piece
{
private:

public:
    Queen(Square position_);
    virtual ~Queen();
    virtual U64 GetAttacks(U64 occupancy_) const; // pure virtual
};