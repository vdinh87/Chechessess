#include "ChessGame.cpp"

int main()
{
    // U64 bb = 1ULL;
    // PrintBoard(bb);
    // std::cout << std::endl;
    ChessGame cg;
    cg.PrintBoard();

    U64 occ = 0ULL;
    Rook rk = Rook(d4);
    PrintBoard( rk.GetAttacks(occ) );
    Knight kn = Knight(e4);
    PrintBoard( kn.GetAttacks(occ) );
}