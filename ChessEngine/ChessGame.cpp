#include "ChessGame.hpp"
#include <cstdlib>
#include <algorithm>
#include <iostream>

ChessGame::ChessGame(/* args */)
{
    // { Pawn, Knight, Bishop, Rook, Queen, King }
    WhitePiecesArray =
        {
            0x000000000000FF00ULL, // WPstartingpositions --> (wpsp & ~board) tihs gives you the valid starting positions
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
    Color color = GetColor(pawn);
    if (color == white)
    {
        attacks = (North(pawn) & ~BlackPieces) | GetEnPassant(square_, occupancy_, color) | (NorthEast(pawn) & BlackPieces & ~FILE_A) | (NorthWest(pawn) & BlackPieces & ~FILE_H);
        // Initial 2 square move
        if (pawn & RANK_2 && !(North(pawn) & board))
            attacks |= pawn << 16 & ~board;
    }

    // black-side moves
    if (color == black)
    {
        attacks = (South(pawn) & ~WhitePieces) | GetEnPassant(square_, occupancy_, color) | (SouthEast(pawn) & WhitePieces & ~FILE_A) | (SouthWest(pawn) & WhitePieces & ~FILE_H);
        // Initial 2 square move
        if (pawn & RANK_7 && !(South(pawn) & board))
            attacks |= pawn >> 16 & ~board;
    }

    return attacks;
}

U64 ChessGame::GetEnPassant(Square square, const U64 occupancy_, Color color) const
{
    U64 enpassant_moves = 0ULL;
    // set_bit(enpassant_moves, square);
    if (EnPassant(square, Pawn, color))
    {
        if (color == white)
            set_bit(enpassant_moves, GetPreviousMove().to + 8);
        else if (color == black)
            set_bit(enpassant_moves, GetPreviousMove().to - 8);
    }
    return enpassant_moves;
}

bool ChessGame::EnPassant(Square square, Piece type, Color color) const
{
    if ((type == Pawn && GetPreviousMove().type == Pawn) &&                                                     // both are pawns
        (abs(GetPreviousMove().to / 8 - GetPreviousMove().from / 8) == 2) &&                                    // moved two places previously
        (abs((GetPreviousMove().to % 8) - (square % 8)) == 1) &&                                                // is next to it
        ((color == white && square >= 32 && square <= 39) || (color == black && square >= 24 && square <= 31))) // check if it's on 4th and 5th rank
        return true;
    return false;
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

    if (!CheckIfMoved(square_))
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

    // Get the color of the piece we're moving
    Color piece_color = GetColor(piece);

    // Create a temporary board to test if moves would leave/put own king in check
    U64 tempBoard = board;
    clear_bit(tempBoard, square_); // Remove piece from current position

    // Filter out moves that would leave/put own king in check
    U64 legal_moves = 0ULL;
    U64 filtered_attacks = FilterTeam(attacks, piece);
    U64 moves = filtered_attacks;

    // Get checking pieces
    U64 checking_pieces = InCheck(board, piece_color, 0);

    while (moves)
    {
        int target_square = get_ls1b_index(moves);
        U64 target = 1ULL << target_square;

        // Save the piece that might be captured
        U64 captured_piece = tempBoard & target;

        // Make the move on temporary board
        U64 temp = tempBoard;
        if (temp & target)   // If capturing a piece
            temp &= ~target; // Remove captured piece
        temp |= target;      // Place piece at new position

        // If this move captures a checking piece or doesn't leave own king in check, it's legal
        if ((captured_piece & checking_pieces) || !InCheck(temp, piece_color, 0))
            set_bit(legal_moves, target_square);

        // Restore captured piece for next iteration
        tempBoard |= captured_piece;

        moves &= moves - 1; // Clear least significant bit
    }

    // Apply the remaining filters
    legal_moves = FilterCheck(legal_moves, piece);
    if (GetPieceType(piece) == King)
        legal_moves = legal_moves & FilterLegalKingMoves(legal_moves, piece);
    else
        legal_moves = legal_moves & FilterPin(legal_moves, piece);

    return legal_moves;
}

Action ChessGame::Promote(Square from_sq, Square to_sq, Color from_color, Piece to_piece)
{
    Piece promoting_to_piece = PromoteInput(from_sq, to_sq, from_color, to_piece);
    ExecuteMove(from_color, from_sq, to_sq, promoting_to_piece, to_piece);
    (from_color == black) ? clear_bit(BlackPiecesArray[Pawn], from_sq) : clear_bit(WhitePiecesArray[Pawn], from_sq);
    return Action::Promotion;
}

U64 ChessGame::GetCastling(Color color) const
{
    bool kingmoved, rookmoved = true;
    U64 valid_moves = 0ULL;
    U64 king = 0ULL;

    if (color == white)
    {
        king = WhitePiecesArray[King];
        kingmoved = CheckIfMoved(e1);
        rookmoved = (CheckIfMoved(a1) | CheckIfMoved(h1));
    }
    else
    {
        king = BlackPiecesArray[King];
        kingmoved = CheckIfMoved(e8);
        rookmoved = (CheckIfMoved(a8) | CheckIfMoved(h8));
    }
    Square king_sq = static_cast<Square>(get_LSB(king));

    if (!kingmoved && !rookmoved &&
        !(get_bit(board, king_sq - 1)) && !(get_bit(board, king_sq - 2)) && !(get_bit(board, king_sq - 3)) &&              // Makes sure it's empty
        (InCheck(board, color, -1) == 0ULL) && (InCheck(board, color, -2) == 0ULL) && (InCheck(board, color, -3) == 0ULL)) // Checking
        set_bit(valid_moves, king_sq - 2);
    if (!kingmoved && !rookmoved &&
        !(get_bit(board, king_sq + 1)) && !(get_bit(board, king_sq + 2)) &&       // Makes sure it's empty
        (InCheck(board, color, 1) == 0ULL) && (InCheck(board, color, 2) == 0ULL)) // Checking
        set_bit(valid_moves, king_sq + 2);

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

    // Check all piece types including Queen
    for (int i = Pawn; i <= Queen; i++) // Changed to <= Queen to include Queen
    {
        attacks = GetAttacks(king_sq, occupany_, i);
        if (color_of_king == white)
            opposite_piece = BlackPiecesArray[i];
        else // black
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
    if (!checkers)
        return moveset; // If not in check, all moves are legal

    // Count the number of checking pieces
    U64 checkers_copy = checkers;
    int checker_count = 0;
    while (checkers_copy)
    {
        checker_count++;
        checkers_copy &= (checkers_copy - 1);
    }

    // If there's more than one checker, only king moves are allowed
    if (checker_count > 1 && !(piece & PieceTypeArray[King]))
    {
        return 0ULL;
    }

    // For a single checker, we can:
    // 1. Capture the checking piece
    // 2. Block the check (if it's a sliding piece)
    // 3. Move the king
    if (piece & PieceTypeArray[King])
    {
        // King moves are already filtered in GetAttacks
        return moveset;
    }

    // For other pieces:
    // 1. Get the square of the checking piece
    Square checker_sq = static_cast<Square>(get_ls1b_index(checkers));
    capture_mask = 1ULL << checker_sq;

    // 2. If it's a sliding piece, add the squares between king and checker
    if (IsSlider(checkers))
    {
        block_mask = GetRay(king_sq, checker_sq);
    }
    else
    {
        block_mask = 0ULL; // Non-sliding pieces can only be captured, not blocked
    }

    return moveset & (capture_mask | block_mask);
}

U64 ChessGame::FilterLegalKingMoves(const U64 &moveset, const U64 &piece) const
{
    if (GetPieceType(piece) != King)
    {
        std::cout << "Error: FilterLegalKingMoves called on non-king piece" << std::endl;
        return 0ULL;
    }

    Color king_color = GetColor(piece);
    Square king_sq = static_cast<Square>(get_ls1b_index(piece));

    // For each potential king move, check if it would put the king in check
    U64 moves = moveset;
    U64 legal_king_moves = 0ULL;

    while (moves)
    {
        Square target_sq = static_cast<Square>(get_ls1b_index(moves));
        U64 target = 1ULL << target_sq;

        // Create a temporary board for this move
        U64 temp_board = board;
        clear_bit(temp_board, king_sq); // Remove king from current position

        // Check if this target square is under attack
        // We need to temporarily remove the king to check if the target square is under attack
        U64 checking_pieces = InCheck(temp_board, king_color, target_sq);

        if (checking_pieces == 0ULL)
        {
            // The target square is not under attack, so this is a legal move
            set_bit(legal_king_moves, target_sq);
        }
        else
        {
            std::cout << "King move to " << target_sq << " is illegal - would be in check from "
                      << __builtin_popcountll(checking_pieces) << " pieces" << std::endl;
        }

        moves &= moves - 1; // Clear least significant bit
    }

    return legal_king_moves;
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

void ChessGame::UpdateBoard()
{
    WhitePieces = WhitePiecesArray[Pawn] | WhitePiecesArray[Knight] | WhitePiecesArray[Bishop] |
                  WhitePiecesArray[Rook] | WhitePiecesArray[Queen] | WhitePiecesArray[King];
    BlackPieces = BlackPiecesArray[Pawn] | BlackPiecesArray[Knight] | BlackPiecesArray[Bishop] |
                  BlackPiecesArray[Rook] | BlackPiecesArray[Queen] | BlackPiecesArray[King];

    AllColorPiecesArray.clear();
    AllColorPiecesArray.push_back(WhitePiecesArray);
    AllColorPiecesArray.push_back(BlackPiecesArray);

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
    Piece p = Pawn; // Default to Pawn to avoid undefined behavior
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (unknown_piece & PieceTypeArray[i])
        {
            p = static_cast<Piece>(i);
            break; // Exit after finding the first match
        }
    }
    return p;
}

bool ChessGame::IsSlider(const U64 board_) const
{
    // A piece is a sliding piece if it's a bishop, rook, or queen
    Piece piece_type = GetPieceType(board_);
    return piece_type == Bishop || piece_type == Rook || piece_type == Queen;
}

bool ChessGame::IsSlider(Piece pieceType) const
{
    // A piece is a sliding piece if it's a bishop, rook, or queen
    return pieceType == Bishop || pieceType == Rook || pieceType == Queen;
}

bool ChessGame::CheckIfMoved(Square original_square) const
{
    for (const auto &move : log.ReadLog())
        if (move.from == original_square)
            return true;
    return false;
}
ChessMove ChessGame::GetPreviousMove() const
{
    if (!log.IsEmpty())
        return log.GetLastMove();
    return ChessMove();
}

void ChessGame::Log(ChessMove move)
{
    log.AddToLog(move);
}

void ChessGame::PrintTheLog()
{
    log.PrintLog();
}
void ChessGame::PrintBoard() const
{
    std::string boardString;
    U64 square;
    std::cout << "Chess Board: " << std::endl;
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

std::vector<Action> ChessGame::Move(Square from_sq, Square to_sq)
{
    std::vector<Action> actions;
    U64 from = 0ULL;
    U64 to = 0ULL;
    set_bit(from, from_sq);
    set_bit(to, to_sq);

    U64 ally_pieces = GetColor(from) == white ? WhitePieces : BlackPieces;

    // no piece on board or parameters are same square
    if (!(from & board) || (from_sq == to_sq) || (to & ally_pieces))
        return actions;

    Color from_color = GetColor(from);
    Piece from_piece = GetPieceType(from);
    Piece to_piece = GetPieceType(to);

    if (from_color == white)
    { // Castling Conditions
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {                                              // Castling
            U64 valid_moves = GetCastling(from_color); // does null check and InCheck
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // Checks which way
            if (to_sq == c1 && get_bit(valid_moves, c1))
            {
                ExecuteMove(from_color, a1, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, King);
            }
            else if (to_sq == g1 && get_bit(valid_moves, g1))
                ExecuteMove(from_color, h1, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        } // Promotion conditions
        else if (from_piece == Pawn && to_sq >= 56 && to_sq <= 63)
        { // Promotion
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        } // En passant conditions
        else if (EnPassant(from_sq, from_piece, from_color))
        { // En passant
            ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) + 8), from_piece, to_piece);
            // clear_bit(BlackPiecesArray[to_piece], GetPreviousMove().to);
            RemovePiece(GetPreviousMove().to);
            actions.push_back(Action::Capture);
        }
        else // Normal Move
        {
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // if capturing
            if (board & (1ULL << to_sq))
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }
    else if (from_color == black)
    { // Castling Conditions
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {                                              // Castling
            U64 valid_moves = GetCastling(from_color); // does null check and InCheck
            ExecuteMove(from_color, from_sq, to_sq, King, King);
            // Checks which way
            if (to_sq == c8 && get_bit(valid_moves, c8))
                ExecuteMove(from_color, a8, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, Rook);
            else if (to_sq == g8 && get_bit(valid_moves, g8))
                ExecuteMove(from_color, h8, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        }
        else if (from_piece == Pawn && to_sq >= 0 && to_sq <= 7)
        { // Promotion
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        } // Enpassant conditoins
        else if (EnPassant(from_sq, from_piece, from_color))
        { // Enpassant
            ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) - 8), from_piece, to_piece);
            // clear_bit(WhitePiecesArray[to_piece], GetPreviousMove().to);
            RemovePiece(GetPreviousMove().to);
            actions.push_back(Capture);
        } // Normal move
        else
        {
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // if capturing
            if (board & (1ULL << to_sq))
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }

    from_color = static_cast<Color>(!static_cast<bool>(from_color));
    if (InCheck(board, from_color, 0)) // const U64 &occupany_, Color color_of_king, int offset
        actions.push_back(Check);

    // after move
    UpdateBoard();

    // Check for win regardless of whether it's a capture or not
    if (IsWin(white) || IsWin(black))
        actions.push_back(Checkmate);

    return actions;
}

void ChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    if (color == white)
    {
        clear_bit(BlackPiecesArray[to_piece], to_sq);
        set_bit(WhitePiecesArray[from_piece], to_sq);
        clear_bit(WhitePiecesArray[from_piece], from_sq);
    }
    else if (color == black)
    {
        clear_bit(WhitePiecesArray[to_piece], to_sq);
        set_bit(BlackPiecesArray[from_piece], to_sq);
        clear_bit(BlackPiecesArray[from_piece], from_sq);
    }
}

Piece ChessGame::PromoteInput(Square from_sq, Square to_sq, Color color, Piece to_piece)
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
        return PromoteInput(from_sq, to_sq, color, to_piece);
    }
    return promoting_to_piece;
}

// public getters
U64 ChessGame::GetPiecesOfColor(Color color) const
{
    if (color == white)
        return WhitePieces;
    return BlackPieces;
}

U64 ChessGame::GetBoard() const
{
    return board;
}

bool ChessGame::IsWin(Color color) const
{
    Color opponent = (color == white) ? black : white;
    U64 opponent_king = (opponent == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    U64 opponent_pieces = (opponent == white) ? WhitePieces : BlackPieces;

    // First check if opponent is in check
    U64 checking_pieces = InCheck(board, opponent, 0);
    if (!checking_pieces)
    {
        std::cout << "IsWin: " << (opponent == white ? "White" : "Black") << " is not in check, so not checkmate" << std::endl;
        return false; // Not in check, so definitely not checkmate
    }

    // Debug: Print how many checking pieces
    int checker_count = __builtin_popcountll(checking_pieces);
    std::cout << "IsWin check: " << (opponent == white ? "White" : "Black") << " has "
              << checker_count << " checking pieces" << std::endl;

    // 1. Check if king can escape check by moving
    Square king_sq = static_cast<Square>(get_ls1b_index(opponent_king));
    U64 king_moves = GetAttacks(king_sq);

    if (king_moves)
    {
        std::cout << "King at square " << king_sq << " has " << __builtin_popcountll(king_moves)
                  << " moves, can escape check" << std::endl;
        return false; // King can move out of check
    }
    else
    {
        std::cout << "King at square " << king_sq << " has no legal moves" << std::endl;
    }

    // If there's more than one checking piece, and king can't move, it's checkmate
    if (checker_count > 1)
    {
        std::cout << "Multiple checking pieces and king can't move, checkmate!" << std::endl;
        return true;
    }

    // 2. Check if checking piece can be captured by any other piece
    Square checker_sq = static_cast<Square>(get_ls1b_index(checking_pieces));
    U64 checker_piece = 1ULL << checker_sq;
    Piece checker_piece_type = GetPieceType(checker_piece);

    U64 other_pieces = opponent_pieces & ~opponent_king;

    while (other_pieces)
    {
        Square piece_sq = static_cast<Square>(get_ls1b_index(other_pieces));
        U64 piece = 1ULL << piece_sq;
        Piece pieceType = GetPieceType(piece);

        // Get attacks for this piece
        U64 attacks = GetAttacks(piece_sq);

        // Check if this piece can capture the checking piece
        if (attacks & checking_pieces)
        {
            std::cout << "Piece at square " << piece_sq << " (type " << pieceType << ") can capture the checking piece" << std::endl;
            return false; // Can capture checking piece
        }

        other_pieces &= other_pieces - 1; // Clear least significant bit
    }

    // 3. For sliding pieces, check if any piece can block the check
    if (IsSlider(checker_piece_type))
    {
        // Get the ray between checker and king
        U64 block_ray = GetRay(checker_sq, king_sq) & ~(1ULL << checker_sq) & ~(1ULL << king_sq);

        if (block_ray)
        {
            // Check if any piece can block
            other_pieces = opponent_pieces & ~opponent_king; // Reset to all non-king pieces

            while (other_pieces)
            {
                Square piece_sq = static_cast<Square>(get_ls1b_index(other_pieces));
                U64 piece = 1ULL << piece_sq;
                Piece pieceType = GetPieceType(piece);

                // Get attacks for this piece
                U64 attacks = GetAttacks(piece_sq);

                // Check if this piece can block any square in the ray
                if (attacks & block_ray)
                {
                    std::cout << "Piece at square " << piece_sq << " (type " << pieceType
                              << ") can block the check" << std::endl;
                    return false; // Can block
                }

                other_pieces &= other_pieces - 1; // Clear least significant bit
            }
        }
    }

    // If we get here, no piece can move to escape, capture, or block - it's checkmate
    std::cout << "Checkmate! " << (opponent == white ? "White" : "Black") << " has no legal moves" << std::endl;
    return true;
}

// board editing
bool ChessGame::AddPiece(Square square, Color color, Piece piece)
{
    std::cout << "Test: 4\n";

    U64 p = 0ULL;
    set_bit(p, square);
    if (!(board & p) && color == white)
        WhitePiecesArray[piece] |= p;
    else if (!(board & p) && color == black)
        BlackPiecesArray[piece] |= p;
    else
        return false;
    UpdateBoard();
    return true;
}

bool ChessGame::RemovePiece(Square square)
{
    U64 p = 1ULL << square;
    // no piece on board
    if (!(p & board))
        return false;

    Color color = GetColor(p);
    if (color == white)
        clear_bit(WhitePiecesArray[GetPieceType(p)], square);
    else
        clear_bit(BlackPiecesArray[GetPieceType(p)], square);

    UpdateBoard();
    return true;
}

U64 ChessGame::GetRay(Square from, Square to) const
{
    U64 ray = 0ULL;

    // Get ranks and files
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;

    // Calculate rank and file differences
    int rankDiff = toRank - fromRank;
    int fileDiff = toFile - fromFile;

    // If the squares aren't on the same rank, file, or diagonal, there's no ray
    if (rankDiff != 0 && fileDiff != 0 && abs(rankDiff) != abs(fileDiff))
        return 0ULL;

    // Normalize the differences to get direction
    int rankDir = (rankDiff == 0) ? 0 : (rankDiff > 0 ? 1 : -1);
    int fileDir = (fileDiff == 0) ? 0 : (fileDiff > 0 ? 1 : -1);

    // Compute all squares in the ray (excluding 'from' and including 'to')
    int currRank = fromRank + rankDir;
    int currFile = fromFile + fileDir;

    while ((currRank != toRank || currFile != toFile) &&
           currRank >= 0 && currRank < 8 && currFile >= 0 && currFile < 8)
    {
        int squareIndex = currRank * 8 + currFile;
        set_bit(ray, static_cast<Square>(squareIndex));

        currRank += rankDir;
        currFile += fileDir;
    }

    // Add the 'to' square to the ray
    set_bit(ray, to);

    return ray;
}
