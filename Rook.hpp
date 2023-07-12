#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Rook : public Piece
{
private:

public:
    Rook(Square position_, Color color_);
    virtual ~Rook() = default;
    virtual U64 GetAttacks(U64 occupancy_) const;
};
