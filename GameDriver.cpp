#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    //move king to middle of board
    // cg.Move(d2, e5);
    //move piece to checking position
    cg.Move(e1, e4);
    cg.Move(c8, c6);
    cg.Move(d8, d4);
    cg.Move(g8, f6);
    cg.Move(d1, f4);
    cg.PrintBoard();
    PrintGoard( cg.InCheck(white) );
}
