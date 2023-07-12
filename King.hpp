#pragma once
#include "SingleMovePiece.hpp"
#include "Piece.hpp"
enum Direction
{
 right,
 left
};

class King : public Piece
{
private:
 bool has_moved = false;
 Square castling_square_right;
 Square castling_square_left; // Figure out ULL
public:
 King(Square position_, Color color_);
 virtual ~King() = default;
 Square GetCastlingSquare(Direction direction_) const;
 bool GetHasMoved() const;
 virtual U64 GetAttacks(U64 occupany_) const;
};
