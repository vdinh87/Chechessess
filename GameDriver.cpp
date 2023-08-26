#include "ChessGame.cpp"

ChessGame cg;

U64 Select(Color side, Square square)
{
    U64 piece;
    set_bit(piece, square);

    if( !(cg.GetPiecesOfColor(side) && piece) )
}

void Play()
{
    static std::unordered_map<std::string, Square> const table = { 
        {"a1", Square::a1}, {"a2", Square::a2}, {"a3", Square::a3}, {"a4", Square::a4},
        {"a5", Square::a5}, {"a6", Square::a6}, {"a7", Square::a7}, {"a8", Square::a8},
        {"b1", Square::b1}, {"b2", Square::b2}, {"b3", Square::b3}, {"b4", Square::b4},
        {"b5", Square::b5}, {"b6", Square::b6}, {"b7", Square::b7}, {"b8", Square::b8},
        {"c1", Square::c1}, {"c2", Square::c2}, {"c3", Square::c3}, {"c4", Square::c4},
        {"c5", Square::c5}, {"c6", Square::c6}, {"c7", Square::c7}, {"c8", Square::c8},
        {"d1", Square::d1}, {"d2", Square::d2}, {"d3", Square::d3}, {"d4", Square::d4},
        {"d5", Square::d5}, {"d6", Square::d6}, {"d7", Square::d7}, {"d8", Square::d8},
        {"e1", Square::e1}, {"e2", Square::e2}, {"e3", Square::e3}, {"e4", Square::e4},
        {"e5", Square::e5}, {"e6", Square::e6}, {"e7", Square::e7}, {"e8", Square::e8},
        {"f1", Square::f1}, {"f2", Square::f2}, {"f3", Square::f3}, {"f4", Square::f4},
        {"f5", Square::f5}, {"f6", Square::f6}, {"f7", Square::f7}, {"f8", Square::f8},
        {"g1", Square::g1}, {"g2", Square::g2}, {"g3", Square::g3}, {"g4", Square::g4},
        {"g5", Square::g5}, {"g6", Square::g6}, {"g7", Square::g7}, {"g8", Square::g8},
        {"h1", Square::h1}, {"h2", Square::h2}, {"h3", Square::h3}, {"h4", Square::h4},
        {"h5", Square::h5}, {"h6", Square::h6}, {"h7", Square::h7}, {"h8", Square::h8}
    };
  
    std::string input_str;
    Square selected_square;
    U64 move_list;
    while( !cg.IsWin(white) || !cg.IsWin(black) )
    {
        std::cout << "White to move." << std::endl;
        std::cout << "Select piece square: ";

        // Input to square
        
        std::cin >> input_str;

        auto it = table.find(input_str);
        if (it != table.end()) {
            selected_square = it->second;    
        } else { "Invalid square"; }
        
        U64 piece;
        set_bit(piece, selected_square);
        while( !(cg.GetPiecesOfColor(white) && piece) )
            std::cout << "Invalid square or chose piece of opposite color." << std::endl;
            
        move_list = Select(white, selected_square);
    }

    if( cg.IsWin(white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
}

int main()
{
    ChessGame cg;
    cg.Move(d8, f2);
    cg.Move(h8, f3);

    cg.PrintBoard();
    std::cout << "White win: " << cg.IsWin(white) << std::endl;
    std::cout << "Black win: " << cg.IsWin(black) << std::endl;
}
