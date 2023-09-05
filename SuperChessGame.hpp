#include "ChessGame.hpp"


class SuperChessGame : public ChessGame
{
private:
    SuperChessGame(Piece 1st_super_piece);
    vector<> effect_list_;

    void AddPiece(Square square, Piece piece);
    void RemovePiece(Square square, Piece piece);

public:
};