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

    Square dest = d5;


    // cg.Move(a1, a5); // random rook move
    // cg.Move(d7, d4); // setting up pawn move
    // cg.Move(e2, e4); // Double pawn move

    // U64 from = 0ULL;
    // set_bit(from, cg.prevMove.from);
    // U64 to = 0ULL;
    // set_bit(to, cg.prevMove.to);

    // std::cout << "Is En passant Available: " << cg.EnPassant(d4, Pawn, black);
    // std::cout << "\n";

    cg.Move(d2,dest);
    cg.Move(e7, e5);
    U64 test = 0ULL;
    set_bit(test, dest);

    cg.PrintBoard();
    std::cout<<"printgoard\n";
    PrintGoard(cg.GetPawnAttacks(dest, occ));



    //PrintGoard(8ULL << 1);
}
