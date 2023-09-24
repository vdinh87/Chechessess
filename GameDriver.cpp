#include "ChessGame.cpp"
#include "SuperChessGame.cpp"
#include "Logger.cpp"

void Play()
{
    ChessGame* cg = new ChessGame();
    Logger chess_game_logger;
    chess_game_logger.AddListener(cg);
    
    while( !cg->IsWin(white) && !cg->IsWin(black) )
    {
        ChessMove move;

        for( int i = white; i <= black && ( !cg->IsWin(white) && !cg->IsWin(black) ); i++)
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
                while( !(cg->GetPiecesOfColor(color) & square) )
                {
                    cg->PrintBoard();
                    std::cout << ColorStrings[color] << " to move." << std::endl;
                    std::cout << "Select valid square: " ;
                
                    // Input to square
                    std::cin >> input_str;
                    std::cout << std::endl;

                    auto it = SqStrMap.find(input_str);
                    if (it != SqStrMap.end())
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
                    move_list = cg->GetAttacks(selected_square);
                    std::cout << "Movelist of piece at square: " << input_str << std::endl;
                    PrintGoard(move_list);
                }   
            }

            //move piece
            move.type = cg->GetPieceType(1ULL << piece);
            move.from = piece;
            move.to = move_to_square;
            std::vector<Action> action_list = cg->Move( piece, move_to_square);
                 
            if( cg->IsWin(white) || cg->IsWin(black) )
            {
                action_list.push_back(Action::Checkmate);
                move.actions = action_list;
                chess_game_logger.Log(move);
                break;
            }

            move.actions = action_list;
            chess_game_logger.Log(move);
        }
    }
    
    if( cg->IsWin(white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
    chess_game_logger.PrintLog();

    delete cg;
}

int main()
{
    Play();
    // SuperPieceInfo KingShot = std::make_pair(King, T3);
    // SuperChessGame* spg = new SuperChessGame(KingShot);
    // spg->PrintBoard();
    // spg->AddPiece(d4, black, Pawn);
    // spg->Move(a2,a5);
    // spg->PrintBoard();
    // spg->UseAbility();
    // spg->PrintBoard();
    // std::cout << "| White   | Black " << std::endl; 
    // //moving regularly
    // std::cout << "| d4      | e5    " << std::endl; //pawn
    // std::cout << "| Ne4     | Be4   " << std::endl; //non-pawn
    // //capture
    // std::cout << "| dxe4    | exf5  " << std::endl; //pawn
    // std::cout << "| Nxe4    | Qxf5  " << std::endl; //non-pawn
    // //check
    // std::cout << "| Qd5+    | Ke4   " << std::endl; //non-pawn
    // //check with capture
    // std::cout << "| gxd5+   | Ke4   " << std::endl; //pawn
    // std::cout << "| Qxd5+   | Ke4   " << std::endl; //non-pawn
    // //promotion
    // std::cout << "| d1=Q    | Ke4   " << std::endl; //pawn
    // //promotion with capture
    // std::cout << "| gxd1=Q  | Ke4   " << std::endl; //pawn
    // //promotion with check
    // std::cout << "| d1=Q+   | Ke4   " << std::endl; //pawn
    // //promotion with capture with check
    // std::cout << "| gxd1=Q+ | Ke4   " << std::endl; //pawn
    // //castling
    // std::cout << "| O-O     | Ke4   " << std::endl; //short castle
    // std::cout << "| O-O-O   | Ke4   " << std::endl; //long castle
    // //castling with check
    // std::cout << "| O-O+    | Ke4   " << std::endl; //short castle
    // std::cout << "| O-O-O+  | Ke4   " << std::endl; //long castle
    // //checkmate
    // std::cout << "| Rd4#    |       " << std::endl; 
    // //checkmate with capture
    // std::cout << "| Rxd4#   |       " << std::endl; 
    // //checkmate with castle
    // std::cout << "| O-O#    |       " << std::endl; //short castle
    // std::cout << "| O-O-O#  |       " << std::endl; //long castle

    // std::cout << std::endl;

    // int width = 8; // Set the desired width
    // std::string text = "Hello"; // Your string

    // // Left-aligned
    // std::cout << "| " << std::setw(width) << std::left << text ;
    // std::cout << "| " << std::setw(width) << std::left << text << std::endl;
    // std::cout << "| " << std::setw(width) << std::left << text ;
    // std::cout << "| " << std::setw(width) << std::left << text << std::endl;
}
