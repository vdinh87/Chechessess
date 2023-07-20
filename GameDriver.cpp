#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    cg.Move(d8, c5);
    cg.Move(e1, e3);
    cg.PrintBoard();
    std::cout << "CheckStatus: "<< cg.InCheck(white) << "\n";
    //PrintBoard( cg.InCheck(white) );    
}