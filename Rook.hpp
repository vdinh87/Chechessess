#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Rook : Piece
{
private:

public:
    Rook(U64 position_);
    virtual ~Rook();
    virtual U64 GetAttacks() const;
};
