#include "ChessGame.hpp"

// PrintBoard with a g
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

    bool has_moved = false; // won't be needed when it's a private member or something
    if (has_moved)          // this way it doesn't have to do the function every single time it just checks one byte
        attacks = attacks | GetCastling(GetColor(king));

    return attacks;
}

U64 ChessGame::GetAttacks(Square square_, const U64 occupancy_, int which_function) const
{
    U64 attacks = 0ULL;

    switch (which_function)
    {
    case 0:
        attacks = GetPawnAttacks(square_, occupancy_);
        break;
    case 1:
        attacks = GetKnightAttacks(square_, occupancy_);
        break;
    case 2:
        attacks = GetBishopAttacks(square_, occupancy_);
        break;
    case 3:
        attacks = GetRookAttacks(square_, occupancy_);
        break;
    case 4:
        attacks = GetQueenAttacks(square_, occupancy_);
        break;
    case 5:
        attacks = GetKingAttacks(square_, occupancy_);
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
    if (!(piece & board))
        throw std::invalid_argument("No piece on square");

    int which_function = -1;
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (piece & PieceTypeArray[i])
            which_function = i;
    }

    attacks = GetAttacks(square_, board, which_function);
    attacks = FilterTeam(attacks, piece);
    attacks = FilterCheck(attacks, piece);
    if (GetPieceType(piece) == King)
        attacks = attacks & FilterLegalKingMoves(attacks, piece);
    else
        attacks = attacks & FilterPin(attacks, piece);

    return attacks;
}

U64 ChessGame::GetCastling(Color color) const
{
    bool kingmoved, rookmoved = false;
    U64 valid_moves = 0ULL;
    U64 king = 0ULL;

    if (color == white)
        king = WhitePiecesArray[King];
    else
        king = BlackPiecesArray[King];
    Square king_sq = static_cast<Square>(get_LSB(king));

    if (!(kingmoved) && !(rookmoved) &&
        !(get_bit(board, king_sq - 1)) && !(get_bit(board, king_sq - 2)) &&         // Makes sure it's empty
        (InCheck(board, white, -1) == 0ULL) && (InCheck(board, white, -2) == 0ULL)) // Checking
    {
        set_bit(valid_moves, king_sq - 2);
    }
    if (!(kingmoved) && !(rookmoved) &&
        !(get_bit(board, king_sq + 1)) && !(get_bit(board, king_sq + 2)) &&       // Makes sure it's empty
        (InCheck(board, white, 1) == 0ULL) && (InCheck(board, white, 2) == 0ULL)) // Checking
    {
        set_bit(valid_moves, king_sq + 2);
    }

    return valid_moves;
}

// Checking functions
U64 ChessGame::InCheck(const U64 &occupany_, Color color_of_king, int offset) const
{
    U64 attacks, king, checking_pieces = 0ULL;
    if (color_of_king == white)
        king = WhitePiecesArray[King];
    else
        king = BlackPiecesArray[King];

    Square king_sq = static_cast<Square>(get_LSB(king) + offset);
    U64 opposite_piece = 0ULL;
    for (int i = Pawn; i <= King; i++)
    {
        attacks = GetAttacks(king_sq, occupany_, i);
        if (color_of_king == white)
            opposite_piece = BlackPiecesArray[i];
        else
            opposite_piece = WhitePiecesArray[i];

        if (attacks & opposite_piece)
            checking_pieces |= (attacks & opposite_piece);
    }
    return checking_pieces;
}

U64 ChessGame::InCheck(const U64 &occupany_, Color color_of_king, const U64 &king) const
{
    U64 attacks, checking_pieces = 0ULL;

    Square king_sq = static_cast<Square>(get_LSB(king));
    U64 opposite_piece = 0ULL;
    for (int i = Pawn; i <= King; i++)
    {
        attacks = GetAttacks(king_sq, occupany_, i);
        if (color_of_king == white)
            opposite_piece = BlackPiecesArray[i];
        else
            opposite_piece = WhitePiecesArray[i];

        if (attacks & opposite_piece)
            checking_pieces |= (attacks & opposite_piece);
    }
    return checking_pieces;
}

// Filter functions
U64 ChessGame::FilterTeam(const U64 &moveset, const U64 &piece) const
{
    Color color = GetColor(piece);
    U64 filtered_moveset = 0ULL;
    if (color == white)
        filtered_moveset = moveset & ~WhitePieces;
    else
        filtered_moveset = moveset & ~BlackPieces;
    return filtered_moveset;
}

U64 ChessGame::FilterCheck(const U64 &moveset, const U64 &piece) const
{
    Color color = GetColor(piece);
    Square king_sq = color ? GetSquare(BlackPiecesArray[King]) : GetSquare(WhitePiecesArray[King]);
    U64 capture_mask = ~0ULL;
    U64 block_mask = ~0ULL;

    U64 checkers = InCheck(board, color, 0);
    bool two_or_more_checkers = false;
    U64 checkers2 = checkers;
    std::vector<Square> checker_locations;

    for (int i = 0; checkers2; i++)
    {
        if (i == 1)
            two_or_more_checkers = true;
        U64 lsb = get_LSB(checkers2);
        checker_locations.push_back(static_cast<Square>(lsb));
        checkers2 &= (checkers2 - 1);
    }

    // may need to be updated as we add filters
    bool is_king = piece & PieceTypeArray[King];

    if (two_or_more_checkers || is_king)
    { // Only king moves allowed.
        U64 checker_attacks = 0ULL;
        for (const Square &square : checker_locations)
        {
            U64 checkerboard = 0ULL;
            set_bit(checkerboard, square);
            Piece type = GetPieceType(checkerboard);
            checker_attacks = GetAttacks(square, 0ULL, type) | checker_attacks;
        }

        return moveset & ~checker_attacks;
    }
    else
    {
        capture_mask = checkers;
        block_mask = 0ULL;
        if (IsSlider(checkers))
            block_mask = GetRay(king_sq, GetSquare(checkers));
    }
    if (!checkers)
        return moveset;
    return moveset & (block_mask | capture_mask);
}

U64 ChessGame::FilterPin(const U64 &moveset, const U64 &piece) const
{
    if (GetPieceType(piece) == King)
        throw std::invalid_argument("King cannot be pinned");
    U64 tempBoard = board;
    // Take piece off board
    clear_bit(tempBoard, GetSquare(piece));

    U64 checker = InCheck(tempBoard, GetColor(piece), 0);
    // not pinned
    if (!checker)
        return moveset;

    // Two checkers => no moves
    U64 two_checkers = checker;
    clear_bit(two_checkers, GetSquare(checker));
    if (two_checkers)
        return 0ULL;

    // Pinned
    U64 king = GetColor(piece) ? BlackPiecesArray[King] : WhitePiecesArray[King];

    U64 pin_moves = GetRay(GetSquare(checker), GetSquare(king));

    // Piece isn't in pin ray
    if (!(piece & pin_moves))
        return moveset;

    // Checker is attackable
    if (moveset & checker)
        set_bit(pin_moves, GetSquare(checker));

    return pin_moves;
}

U64 ChessGame::FilterLegalKingMoves(const U64 &moveset, const U64 &piece) const
{
    if (GetPieceType(piece) != King)
        throw std::invalid_argument("Can't filter legal king moves: Not a king");
    U64 illegal_moveset = 0ULL;
    U64 moveset_ = moveset;

    while (moveset_)
    {
        int lsb = get_ls1b_index(moveset_);
        U64 temp_king = 0ULL;
        set_bit(temp_king, lsb);

        U64 temp_board = board;
        clear_bit(temp_board, GetSquare(piece));
        set_bit(temp_board, lsb);

        if (InCheck(temp_board, GetColor(piece), temp_king))
            set_bit(illegal_moveset, lsb);

        moveset_ &= moveset_ - 1;
    }
    return moveset & ~illegal_moveset;
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

bool ChessGame::IsSlider(const U64 board_) const
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

    // no piece on board or parameters are same square
    if (!(from & board) || (from_sq == to_sq) || (to & ally_pieces))
        return;

    Color from_color = GetColor(from);
    Piece from_piece = GetPieceType(from);
    Piece to_piece = GetPieceType(to);

    if (from_color == white)
    {

        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
            Castle(from_sq, to_sq, GetCastling(from_color));
        else if (from_piece == Pawn && (to_sq >= 0 && to_sq <= 7) || (to_sq >= 56 && to_sq <= 63))
            Promote(from_sq, to_sq, white, to_piece);
        else
        {
            clear_bit(BlackPiecesArray[to_piece], to_sq);
            set_bit(WhitePiecesArray[from_piece], to_sq);
            clear_bit(WhitePiecesArray[from_piece], from_sq);
        }
    }
    else
    {
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
            Castle(from_sq, to_sq, GetCastling(from_color));
        else if (from_piece == Pawn && (to_sq >= 0 && to_sq <= 7) || (to_sq >= 56 && to_sq <= 63))
            Promote(from_sq, to_sq, black, to_piece);
        else
        {
            clear_bit(WhitePiecesArray[to_piece], to_sq);
            set_bit(BlackPiecesArray[from_piece], to_sq);
            clear_bit(BlackPiecesArray[from_piece], from_sq);
        }
    }

    UpdateBoard();
}

void ChessGame::Castle(Square from_sq, Square to_sq, U64 valid_moves)
{
    U64 from = 0ULL;
    set_bit(from, from_sq);
    Color from_color = GetColor(from); // could change param to 1ULL << from_sq maybe.

    if (from_color == white)
    {
        if ((to_sq == c1 && get_bit(valid_moves, c1)) || (to_sq == c8 && get_bit(valid_moves, c8)))
        {
            set_bit(WhitePiecesArray[King], to_sq);
            clear_bit(WhitePiecesArray[King], from_sq);
            set_bit(WhitePiecesArray[Rook], to_sq + 1);
            clear_bit(WhitePiecesArray[Rook], a1);
        }
        else if ((to_sq == g1 && get_bit(valid_moves, g1)) || (to_sq == g8 && get_bit(valid_moves, g8)))
        {
            set_bit(WhitePiecesArray[King], to_sq);
            clear_bit(WhitePiecesArray[King], from_sq);
            set_bit(WhitePiecesArray[Rook], to_sq - 1);
            clear_bit(WhitePiecesArray[Rook], h1);
        }
    }
    else
    {
        if ((to_sq == c1 && get_bit(valid_moves, c1)) || (to_sq == c8 && get_bit(valid_moves, c8)))
        {
            set_bit(WhitePiecesArray[King], to_sq);
            clear_bit(WhitePiecesArray[King], from_sq);
            set_bit(WhitePiecesArray[Rook], to_sq + 1);
            clear_bit(WhitePiecesArray[Rook], a1);
        }
        else if ((to_sq == g1 && get_bit(valid_moves, g1)) || (to_sq == g8 && get_bit(valid_moves, g8)))
        {
            set_bit(WhitePiecesArray[King], to_sq);
            clear_bit(WhitePiecesArray[King], from_sq);
            set_bit(WhitePiecesArray[Rook], to_sq - 1);
            clear_bit(WhitePiecesArray[Rook], h1);
        }
    }

    UpdateBoard();
}

void ChessGame::Promote(Square from_sq, Square to_sq, Color color, Piece to_piece)
{
    char promotionChoice;
    std::cout << "Pawn promotion: Choose Q for Queen, R for Rook, B for Bishop, or N for Knight: ";
    std::cin >> promotionChoice;

    Piece promoting_to_piece;

    switch (promotionChoice)
    {
    case 'Q':
    case 'q':
        promoting_to_piece = Queen;
        break;
    case 'R':
    case 'r':
        promoting_to_piece = Rook;
        break;
    case 'B':
    case 'b':
        promoting_to_piece = Bishop;
        break;
    case 'N':
    case 'n':
        promoting_to_piece = Knight;
        break;
    default:
        std::cout << "Invalid choice." << std::endl;
    }

    //choose stdin::cin and setbit according to piece cined, clearbit from before
    if (color == white)
    {
        clear_bit(BlackPiecesArray[to_piece], to_sq);
        set_bit(WhitePiecesArray[promoting_to_piece], to_sq);
        clear_bit(WhitePiecesArray[Pawn], from_sq);
    }
    else
    {
        clear_bit(WhitePiecesArray[to_piece], to_sq);
        set_bit(BlackPiecesArray[promoting_to_piece], to_sq);
        clear_bit(BlackPiecesArray[Pawn], from_sq);
    }
    UpdateBoard();
}
