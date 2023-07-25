#include "ChessGame.cpp"

int main()
{
    U64 occ = 0ULL;
    std::vector<U64> v = { occ };
    ChessGame cg;
    //move king to middle of board
    // cg.Move(d2, e5);
    //move piece to checking position
    cg.PrintBoard();

    // std::cout << "Slider status: " << cg.isSlider(d8) << std::endl;
}
