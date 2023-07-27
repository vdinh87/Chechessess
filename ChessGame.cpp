#include "ChessGame.hpp"

//PrintBoard with a g
void PrintGoard(U64 board)
{
    std::string boardString;
    U64 square;
    for (int i = 63; i >= 0; i--)
    {
        square = get_bit(board, i);
        if (square)
            boardString += "1 ";
        else
            boardString += "0 ";

        // new line + reverse
        if ((i % 8) == 0)
        {
            boardString += " " + std::to_string(i / 8 + 1);
            std::reverse(boardString.begin(), boardString.end());
            std::cout << boardString << std::endl;
            boardString.clear();
        }
    }
    std::cout << "   a b c d e f g h\n"
              << std::endl;
}

ChessGame::ChessGame(/* args */)
{
    // { Pawn, Knight, Bishop, Rook, Queen, King }
    WhitePiecesArray =
        {
            0x000000000000FF00ULL,
            0x0000000000000042ULL,
            0x0000000000000024ULL,
            0x0000000000000081ULL,
            0x0000000000000008ULL,
            0x0000000000000010ULL};

    BlackPiecesArray =
        {
            0x00FF000000000000ULL,
            0x4200000000000000ULL,
            0x2400000000000000ULL,
            0x8100000000000000ULL,
            0x0800000000000000ULL,
            0x1000000000000000ULL};

    for (size_t i = 0; i < WhitePiecesArray.size(); i++)
        PieceTypeArray.push_back(WhitePiecesArray[i] | BlackPiecesArray[i]);

    InitMagics();
    UpdateBoard();
}

void ChessGame::InitMagics() const
{
    init_sliders_attacks(0);
    init_sliders_attacks(1);
}

U64 ChessGame::Mask(Piece piece, Color color, const U64 &other_board) const
{
    if (color)
        return other_board & WhitePiecesArray[piece];
    // black
    return other_board & BlackPiecesArray[piece];
}
/* ATTACK FUNCTIONS */
U64 ChessGame::GetPawnAttacks(Square square_, const U64 occupancy_) const
{
    U64 attacks, pawn = 0ULL;
    set_bit(pawn, square_);
    // white-side moves
    if (GetColor(pawn) == white)
    {
        attacks = (North(pawn) | NorthWest(pawn) | NorthEast(pawn));

        // Initial 2 square move
        if (pawn & RANK_2)
            attacks |= pawn << 16;
    }

    // black-side moves
    if (GetColor(pawn) == black)
    {
        attacks = (South(pawn) | SouthWest(pawn) | SouthEast(pawn));

        // Initial 2 square move
        if (pawn & RANK_7)
            attacks |= pawn >> 16;
    }

    return attacks;
}

U64 ChessGame::GetKnightAttacks(Square square_, const U64 occupancy_) const
{
    U64 attacks, knight = 0ULL;
    set_bit(knight, square_);
    attacks = (((knight & (FILE_G | FILE_H)) ? 0ULL : (knight >> 6) | (knight << 10)) |
               ((knight & (FILE_A | FILE_B)) ? 0ULL : (knight >> 10) | (knight << 6)) |
               ((knight & FILE_H) ? 0ULL : (knight >> 15 | knight << 17)) |
               ((knight & FILE_A) ? 0ULL : (knight >> 17 | knight << 15)));

    return attacks; // Queried with U64 attacks = Knightattacks[from] & targets;
}

U64 ChessGame::GetBishopAttacks(Square square_, const U64 occupancy_) const
{
    return get_bishop_attacks(square_, occupancy_);
}

U64 ChessGame::GetRookAttacks(Square square_, const U64 occupancy_) const
{
    return get_rook_attacks(square_, occupancy_);
}

U64 ChessGame::GetQueenAttacks(Square square_, const U64 occupancy_) const
{
    return get_bishop_attacks(square_, occupancy_) | get_rook_attacks(square_, occupancy_);
}

U64 ChessGame::GetKingAttacks(Square square_, const U64 occupancy_) const
{
    U64 attacks, king = 0ULL;
    set_bit(king, square_);
    attacks = (North(king) | NorthEast(king) | NorthWest(king) |
               South(king) | SouthEast(king) | SouthWest(king) |
               East(king) | West(king));

    return attacks;
}

U64 ChessGame::GetAttacks(Square square_, const U64 occupancy_, int which_function) const
{
    U64 attacks = 0ULL;
    
    switch (which_function)
    {
    case 0:
        attacks = GetPawnAttacks(square_, board);
        break;
    case 1:
        attacks = GetKnightAttacks(square_, board);
        break;
    case 2:
        attacks = GetBishopAttacks(square_, board);
        break;
    case 3:
        attacks = GetRookAttacks(square_, board);
        break;
    case 4:
        attacks = GetQueenAttacks(square_, board);
        break;
    case 5:
        attacks = GetKingAttacks(square_, board);
        break;
    case -1:
        break;
    }
   
    return attacks;
}

U64 ChessGame::GetAttacks(Square square_) const
{
    U64 attacks, piece = 0ULL;
    set_bit(piece, square_);

    int which_function = -1;
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (piece & PieceTypeArray[i])
            which_function = i;
    }
        
    attacks = GetAttacks( square_, board, which_function );

    // Filter out ally pieces from attack
    Color piece_color = ( set_bit(piece, square_) & WhitePieces ) ? white : black; 
    attacks = GetFilteredAttacks(attacks, piece_color);
    return attacks;
}

// Checking functions
U64 ChessGame::InCheck(Color color_of_king) const
{
    U64 attacks, king, checking_pieces = 0ULL;
    if( color_of_king == white )
        king = WhitePiecesArray[King];
    else
        king = BlackPiecesArray[King];
    
    Square king_sq = static_cast<Square>(get_LSB(king));
    U64 temp_board = board;    
    U64 opposite_piece = 0ULL;
    for( int i = Pawn; i <= Queen; i++ )
    {
        attacks = GetAttacks( king_sq, temp_board, i);
        if( color_of_king == white )
            opposite_piece = BlackPiecesArray[i];
        else
            opposite_piece = WhitePiecesArray[i];
        
        if( attacks & opposite_piece )
            checking_pieces |= (attacks & opposite_piece);
    }
    return checking_pieces;
}

//Filter functions
U64 ChessGame::GetFilteredAttacks(const U64& moveset, Color color) const
{
    U64 filtered_attacks = FilterTeam(moveset, color);
    // filtered_attacks = FilterCheck(filtered_attacks, color);
    return filtered_attacks;
}

U64 ChessGame::FilterCheck(const U64& moveset, Color color) const
{
    /* VIET CODE
    64 king = 0ULL;
    U64 checker = InCheck(color);

    if( color == white )
        king = WhitePiecesArray[King];
    else
        king = BlackPiecesArray[King];
    
    Square king_sq = static_cast<Square>( get_LSB(king) );
    Square checker_sq = static_cast<Square>( get_LSB(checker) );

    int distance = GetDistance(checker_sq, king_sq);
    U64 ray = GetRay(king, checker, distance);
    */

    U64 checkers = InCheck(color);
    bool two_or_more_checkers = false;
    U64 checkers2 = checkers;
    std::vector<Square> checker_locations;
    Square king_sq;

    if (color == white)
        king_sq = static_cast<Square>(get_LSB(WhitePiecesArray[King]));
    else
        king_sq = static_cast<Square>(get_LSB(BlackPiecesArray[King]));

    for (int i = 0; checkers2; i++)
    {
        if (i == 1)
            two_or_more_checkers = true;
        U64 lsb = get_LSB(checkers2);
        checker_locations.push_back(static_cast<Square>(lsb));
        checkers2 &= (checkers2 - 1);
    }

    U64 capture_mask = ~0ULL;
    U64 block_mask = ~0ULL;

    if (two_or_more_checkers) 
    { //Only king moves allowed.
        if (moveset & FilterTeam(GetAttacks(king_sq), color) )
        {  //If it's a king gets evade_attacks.  Condition may need changing as we add filters.
            U64 checker_attacks = 0ULL;
            for (const Square &square : checker_locations)
            {
                checker_attacks |= GetAttacks(square);
            }
            return moveset & ~checker_attacks;
        }
        return 0ULL;
    }
    else
    {
        capture_mask = checkers;
        // If it's checker is a slider, find rays to king set block_mask = opponent_slider_rays_to_square(king_sq, board); .. If it's not a slider set it to nothing.
        block_mask = 0ULL;
        if (isSlider(checkers))
            block_mask = GetRay(king_sq, static_cast<Square>(get_LSB(checkers)) );
    }

    GetFilteredAttacks(moveset, color);
    return moveset & (block_mask | capture_mask);
}

U64 ChessGame::FilterTeam(const U64& moveset, Color color) const
{
    U64 filtered_moveset = 0ULL;
    if( color == white )
        filtered_moveset = moveset & ~WhitePieces;
    else 
        filtered_moveset = moveset & ~BlackPieces;
    return filtered_moveset;
}

void ChessGame::UpdateBoard()
{
    WhitePieces = WhitePiecesArray[Pawn] | WhitePiecesArray[Knight] | WhitePiecesArray[Bishop] |
                  WhitePiecesArray[Rook] | WhitePiecesArray[Queen] | WhitePiecesArray[King];
    BlackPieces = BlackPiecesArray[Pawn] | BlackPiecesArray[Knight] | BlackPiecesArray[Bishop] |
                  BlackPiecesArray[Rook] | BlackPiecesArray[Queen] | BlackPiecesArray[King];
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
        PieceTypeArray[i] = (WhitePiecesArray[i] | BlackPiecesArray[i]);
    board = WhitePieces | BlackPieces;
}

// Utility functions
Color ChessGame::GetColor(U64 piece) const
{
    if ((piece & WhitePieces) && (piece & BlackPieces))
        throw std::invalid_argument("Inputted board, not piece.");

    if (piece & WhitePieces)
        return white;
    return black;
}

Piece ChessGame::GetPieceType(U64 unknown_piece) const
{
    Piece p;
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (unknown_piece & PieceTypeArray[i])
            p = static_cast<Piece>(i);
    }
    return p;
}

bool ChessGame::isSlider(const U64 board_) const
{
    U64 slider_pieces = WhitePiecesArray[Queen] | WhitePiecesArray[Bishop] | WhitePiecesArray[Rook] | 
                        BlackPiecesArray[Queen] | BlackPiecesArray[Bishop] | BlackPiecesArray[Rook];

    if (board_ & slider_pieces)
        return true;
    return false;
}

void ChessGame::PrintBoard() const
{
    std::string boardString;
    U64 square;
    for (int i = 63; i >= 0; i--)
    {
        square = get_bit(board, i);
        if (square)
        {
            if (Mask(Pawn, white, square) | Mask(Pawn, black, square))
                boardString += "P ";
            if (Mask(Knight, white, square) | Mask(Knight, black, square))
                boardString += "N ";
            if (Mask(Bishop, white, square) | Mask(Bishop, black, square))
                boardString += "B ";
            if (Mask(Rook, white, square) | Mask(Rook, black, square))
                boardString += "R ";
            if (Mask(Queen, white, square) | Mask(Queen, black, square))
                boardString += "Q ";
            if (Mask(King, white, square) | Mask(King, black, square))
                boardString += "K ";
        }
        else
            boardString += "0 ";

        // new line + reverse
        if ((i % 8) == 0)
        {
            boardString += " " + std::to_string(i / 8 + 1);
            std::reverse(boardString.begin(), boardString.end());
            std::cout << boardString << std::endl;
            boardString.clear();
        }
    }
    std::cout << "   a b c d e f g h\n"
              << std::endl;
}

void ChessGame::Move(Square from_sq, Square to_sq)
{
    U64 from = 0ULL;
    U64 to = 0ULL;
    set_bit(from, from_sq);
    set_bit(to, to_sq);

    U64 ally_pieces = GetColor(from) == white ? WhitePieces : BlackPieces;

    //no piece on board or parameters are same square
    if ( !(from & board) || (from_sq == to_sq) || (to_sq & ally_pieces) ) 
        return;

    Color from_color = GetColor(from);
    Piece from_piece = GetPieceType(from);
    Piece to_piece = GetPieceType(to);

    if (from_color == white)
    {
        clear_bit(BlackPiecesArray[to_piece], to_sq);
        set_bit(WhitePiecesArray[from_piece], to_sq);
        clear_bit(WhitePiecesArray[from_piece], from_sq);
    }
    else
    {
        clear_bit(WhitePiecesArray[to_piece], to_sq);
        set_bit(BlackPiecesArray[from_piece], to_sq);
        clear_bit(BlackPiecesArray[from_piece], from_sq);
    }

    UpdateBoard();
}

