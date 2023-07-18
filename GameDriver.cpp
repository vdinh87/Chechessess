#include "ChessGame.cpp"

int main()
{
    // ChessGame cg;
    // cg.PrintBoard();
    U64 occ = 1048576ULL;
    Knight kn = Knight(g5, black);
    Pawn p = Pawn(e4, black);
    Queen r = Queen(c3, black);
    King k = King(d7, black);
    PrintBoard( occ );
}