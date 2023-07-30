#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = {occ};
    ChessGame cg;
    // move king to middle of board
    //  cg.Move(d2, e5);
    // move piece to checking position

    // b6, d6, f6, a4, f4

    Square dest = e3;

    cg.Move(d8, f5); // Queen

    // cg.Move(d8, q);
    // Moving all pieces
    cg.Move(a2, a7);
    cg.Move(b2, b7);
    cg.Move(b1, b8);
    cg.Move(c2, c7);
    cg.Move(c1, c8);
    cg.Move(d2, d7);
    cg.Move(d1, d8);
    cg.Move(e2, e7);
    cg.Move(f2, f7);
    cg.Move(f1, f8);
    cg.Move(g2, g7);
    cg.Move(g1, g8);
    cg.Move(h2, h7);

    cg.Move(f5, h5); // Queen

    cg.PrintBoard();

    cg.Move(e1, g1);

    cg.PrintBoard();

    //    PrintGoard(8ULL << 1);
}
