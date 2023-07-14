#include "ChessGame.cpp"

int main()
{
    // ChessGame cg;
    // cg.PrintBoard();
    U64 occ = 0ULL;
    Knight kn = Knight(g5, black);
    Pawn p = Pawn(e4, black);
    Queen r = Queen(c2, black);
    King k = King(d7, black);
    PrintBoard( k.GetAttacks(occ) );
}