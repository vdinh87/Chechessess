#include "ChessGame.cpp"
#include "SuperChessGame.cpp"
void Play()
{
    ChessGame cg;

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
  
    while( !cg.IsWin(white) && !cg.IsWin(black) )
    { 
        for( int i = white; i <= black && ( !cg.IsWin(white) && !cg.IsWin(black) ); i++)
        {
            Color color = static_cast<Color>(i);
            std::string input_str;
            Square move_to_square = invalid;
            U64 move_list = 0;
            Square piece = invalid;
            while( move_to_square == invalid )
            {
                Square selected_square = invalid;
                U64 square = 0;
                // Invalid selected square loop
                while( !(cg.GetPiecesOfColor(color) & square) )
                {
                    cg.PrintBoard();
                    std::cout << ColorStrings[color] << " to move." << std::endl;
                    std::cout << "Select valid square: " ;
                
                    // Input to square
                    std::cin >> input_str;
                    std::cout << std::endl;

                    auto it = table.find(input_str);
                    if (it != table.end())
                        selected_square = it->second;
                    else 
                        std::cout << "Invalid square" << std::endl;
                    
                    //Selected square is in movelist
                    set_bit(square, selected_square);

                    if( square & move_list )
                    {
                        move_to_square = GetSquare(square);
                        break;
                    }
                    else
                        move_list = 0;
                    //Set piece
                    piece = selected_square;
                }

                if( move_to_square == invalid)
                {
                    move_list = cg.GetAttacks(selected_square);
                    std::cout << "Movelist of piece at square: " << input_str << std::endl;
                    PrintGoard(move_list);
                }   
            }

            //move piece
            cg.Move( piece, move_to_square);
            // if( cg.IsWin(white) || cg.IsWin(black) )
            //     break;
        }
    }
    
    if( cg.IsWin(white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
}

int main()
{
    // Play();
    SuperPieceInfo KingShot = std::make_pair(King, T3);
    SuperChessGame* spg = new SuperChessGame(KingShot);
    spg->PrintBoard();
    spg->AddPiece(d4, black, Pawn);
    spg->Move(a2,a5);
    spg->PrintBoard();
    spg->UseAbility();
    spg->PrintBoard();
}
