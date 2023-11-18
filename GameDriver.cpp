#include "ChessGame.cpp"
#include "SuperChessGame.cpp"
#include "Logger.cpp"

void PlayChessGame()
{
    ChessGame* cg = new ChessGame();
        
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
                cg->Log(move);
                break;
            }
            
            move.actions = action_list;
            cg->Log(move);
        }
    }
    
    if( cg->IsWin(white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
    cg->PrintTheLog();

    delete cg;
}

void PlaySuperChessGame()
{
    ChessGame* spg = new ChessGame();
        
    while( !spg->IsWin(white) && !spg->IsWin(black) )
    {
        ChessMove move;

        for( int i = white; i <= black && ( !spg->IsWin(white) && !spg->IsWin(black) ); i++)
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
                while( !(spg->GetPiecesOfColor(color) & square) )
                {
                    spg->PrintBoard();
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
                    move_list = spg->GetAttacks(selected_square);
                    std::cout << "Movelist of piece at square: " << input_str << std::endl;
                    PrintGoard(move_list);
                }   
            }

            //move piece
            move.type = spg->GetPieceType(1ULL << piece);
            move.from = piece;
            move.to = move_to_square;
            std::vector<Action> action_list = spg->Move( piece, move_to_square);
            if( spg->IsWin(white) || spg->IsWin(black) )
            {
                action_list.push_back(Action::Checkmate);
                move.actions = action_list;
                spg->Log(move);
                break;
            }
            
            move.actions = action_list;
            spg->Log(move);
        }
    }
    
    if( spg->IsWin(white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
    spg->PrintTheLog();

    delete spg;
}

void TestChessGame()
{
    PlayChessGame();
}

void TestSuperChessGame()
{
    SuperPieceInfo info(Piece::Bishop, Tier::T4);
    SuperChessGame* spg = new SuperChessGame();

    spg->ConvertToSuperPiece(info, Square::c1);
    
    // spg->PrintBoard();
    spg->PrintNumAbilities(Square::c1);
}

int main()
{
    // TestChessGame();
    // TestSuperChessGame();
    PlaySuperChessGame();
}

