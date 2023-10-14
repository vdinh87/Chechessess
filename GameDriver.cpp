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
    SuperPieceInfo king = std::make_pair(Piece::King, Tier::T4);
    SuperChessGame* spg = new SuperChessGame(king, king);
    std::vector<std::unique_ptr<Ability>> v;
    spg->MakeAbilityVector( v, std::make_pair(Piece::King, Tier::T3) );
    std::cout << v.empty() << std::endl;
    // spg->ConvertToSuperPiece(king, e1);
    // spg->AddSuperPiece(king, e3, white);
    // spg->AddSuperPiece(king, e3, white);
    // Square k = a1;
    // spg->PrintBoard();
    // spg->ConvertPieceToSide(k, black);
    // spg->ConvertToSuperPiece(king, k);
    // spg->Do(e3, Tier::T4);
    // spg->PrintBoard();
}
