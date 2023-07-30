#include "ChessGame.cpp"

int main()
{
    U64 occ, test = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    //move king to middle of board
    // cg.Move(d2, e5);
    //move piece to checking position
    
    //b6, d6, f6, a4, f4

    Square dest = f4;


    cg.Move(e1, dest); // King
    cg.Move(e8, f6); // Queen
    // cg.Move(f1, f4);
    // cg.Move(h8, e5);
    cg.PrintBoard();
    std::cout << "Piece moves: \n";
    PrintGoard(cg.GetAttacks(f6));
}