#pragma once
#include "Piece.hpp"

class Knight : public Piece
{
private:

public:
    Knight(Square position_, Color color_);
    virtual ~Knight() = default;
    virtual U64 GetAttacks(U64 occupany_) const; // pure virtual
};
