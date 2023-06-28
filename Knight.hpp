#include "Piece.hpp"

class Knight : public Piece
{
private:
    
public:
    Knight(U64 position_);
    virtual ~Knight();
    virtual U64 GetAttacks() const; // pure virtual
};
