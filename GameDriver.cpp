#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    //move king to middle of board
    // cg.Move(d2, e5);
    //move piece to checking position
    
    //b6, d6, f6, a4, f4

    Square dest = d3;

    cg.Move(e1, dest); // King
    cg.Move(d8, f5); // Queen
    cg.Move(h1, h5); // Rook
    cg.Move(f1, f3); // Rook
    // cg.Move(d8, q);
    cg.PrintBoard();

    // cg.GetAttacks(d3);
    std::cout << "Bishop moves: \n";
    PrintGoard(cg.GetAttacks(f3)); //Once it's done debugging
    std::cout << "Rook moves: \n";
    PrintGoard(cg.GetAttacks(h5)); //Once it's done debugging
}
