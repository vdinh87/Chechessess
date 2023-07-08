#include "ChessGame.cpp"

int main()
{
    // U64 bb = 1ULL;
    // PrintBoard(bb);
    // std::cout << std::endl;
    ChessGame cg;
    cg.PrintBoard();

    U64 occ = 0ULL;
    Knight kn = Knight(h5);
    PrintBoard( kn.GetAttacks(occ) );
}