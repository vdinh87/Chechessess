#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    cg.Move(e1, e3);
    cg.PrintBoard();
    PrintBoard( cg.GetAttacks(e3) );    
}