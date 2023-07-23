#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    //move king to middle of board
    cg.Move(e1, e4);
    //move piece to checking position
    cg.Move(h8, a4);

    cg.PrintBoard();
    std::cout << "CheckStatus: \n"<< cg.InCheck(white) << "\n";
}
