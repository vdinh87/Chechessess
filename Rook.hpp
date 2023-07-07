#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Rook : public Piece
{
private:

public:
    Rook(Square position_);
    virtual ~Rook();
    virtual U64 GetAttacks(U64 occupancy_) const;
};
