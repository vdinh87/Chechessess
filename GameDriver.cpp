#include "ChessGame.cpp"

int main()
{
    // ChessGame cg;
    // cg.PrintBoard();

    U64 occ = 0ULL;
    Knight kn = Knight(g5, white);
    Pawn p = Pawn(b4, white);
    PrintBoard( p.GetAttacks(occ) );
}