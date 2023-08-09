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


    cg.Move(f5, h5); // Queen

    Square to_sq = a1;

    cg.PrintBoard();

    cg.Move(a7, a1);

    cg.PrintBoard();
    //    PrintGoard(8ULL << 1);
}
