#include "ChessGame.cpp"
#include "SuperChessGame.cpp"
#include "Logger.cpp"

void PlayChessGame()
{
    std::unique_ptr<ChessGame> cg = std::make_unique<ChessGame>();
        
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
}

void PlaySuperChessGame(SuperPieceInfo white_info, SuperPieceInfo black_info)
{
    std::unique_ptr<SuperChessGame> spg = std::make_unique<SuperChessGame>(white_info, black_info);
        
    while( !spg->IsWin(Color::white) && !spg->IsWin(black) )
    {
        ChessMove move;

        for( int i = Color::white; i <= Color::black && ( !spg->IsWin(Color::white) && !spg->IsWin(Color::black) ); i++)
        {
            Color color = static_cast<Color>(i);
            std::string input_str;
            Square move_to_square = invalid;
            bool ability_selected = false;
            bool is_super = false;
            U64 move_list = 0;
            Square piece = invalid;
            std::vector<Action> action_list;
            while( move_to_square == invalid && !ability_selected)
            {
                Square selected_square = invalid;
                U64 square = 0;
                // Invalid selected square loop
                while( !(spg->GetPiecesOfColor(color) & square) )
                {
                    spg->PrintBoard();
                    
                    std::cout << ColorStrings[color] << " to move." << std::endl;
                    
                    if( is_super )
                        std::cout << "Select valid square or ability: " ;
                    else
                        std::cout << "Select valid square: " ;

                    // Input
                    std::cin >> input_str;
                    std::cout << std::endl;
                    
                    auto it = SqStrMap.find(input_str);
                    if (it != SqStrMap.end())
                        selected_square = it->second;
                    else if( is_super && (std::stoi(input_str) >= 0 && (std::stoi(input_str) <= spg->GetNumAbilities(piece))) )
                    {
                        ability_selected = true;
                        break;
                    }
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

                if( move_to_square == invalid && !ability_selected)
                {
                    move_list = spg->GetAttacks(selected_square);
                    std::cout << "Movelist of piece at square: " << input_str << std::endl;
                    PrintGoard(move_list);
                    //Ability prompt
                    is_super = spg->IsSuperPiece(selected_square);
                    if( is_super )
                    {
                        std::cout << "Abilities: ";
                        spg->PrintAbilityNames(selected_square);
                    }
                }

                //ability 
                if( ability_selected )
                {
                    move.type = spg->GetPieceType(piece);
                    move.from = piece;
                    move.to = Square::invalid;
                    action_list = {Action::Abilityes};

                    Tier t = static_cast<Tier>( std::stoi(input_str) );

                    bool ability_success = spg->UseAbility(piece, t);
                    if( ability_success )
                    {
                        break;
                    }
                    else
                    {
                        ability_selected = false;
                        std::cout << "Ability failed" << std::endl;
                    }
                }
            }

            if( !ability_selected )
            {
                move.type = spg->GetPieceType(1ULL << piece);
                move.from = piece;
                move.to = move_to_square;
                action_list = spg->Move( piece, move_to_square);
            }

            // checkmate actions
            if( spg->IsWin(Color::white) || spg->IsWin(black) )
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
    
    if( spg->IsWin(Color::white) )
        std::cout << "White win!" << std::endl;
    else
        std::cout << "Black win!" << std::endl;
    spg->PrintTheLog();
}

void TestChessGame()
{
    PlayChessGame();
}

void TestSuperChessGame()
{

}

int main()
{
    // TestChessGame();
    // TestSuperChessGame();
    SuperPieceInfo info = std::make_pair(Piece::King, Tier::T3);
    PlaySuperChessGame( info, info );

}