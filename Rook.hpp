#pragma once
#include "Piece.hpp"
#include "Magics.cpp"

class Rook : public Piece
{
private:
    bool has_moved = false;
public:
    Rook(Square position_, Color color_);
    virtual ~Rook() = default;
    virtual void SetPosition(Square new_position);
    bool GetHasMoved() const;
    virtual U64 GetAttacks(U64 occupancy_) const;
};
