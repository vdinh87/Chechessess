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


    cg.Move(d8, f3);
    cg.Move(c8, g3);
    cg.Move(b8, b3);

    cg.PrintBoard();

    std::cout << "Printgoard castling: \n";
    PrintGoard(cg.GetAttacks(e8));

    std::cout << "Castling\n";
    cg.Move(e8, c8);
    cg.PrintBoard();


    //PrintGoard(8ULL << 1);
}
