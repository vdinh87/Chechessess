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

    Square dest = e3;

    cg.Move(e1, dest); // King
    cg.Move(d8, f5); // Queen

    // cg.Move(d8, q);
    cg.PrintBoard();

    // cg.GetAttacks(d3); yes
    std::cout << "King moves: \n";
    PrintGoard(cg.GetAttacks(dest)); 
}
