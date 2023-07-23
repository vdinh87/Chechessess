#include "ChessGame.hpp"

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
U64 ChessGame::GetPawnAttacks(U64 pawn, const U64 occupancy_) const
{
    U64 attacks = 0ULL;

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

U64 ChessGame::GetKnightAttacks(U64 knight, const U64 occupancy_) const
{
    U64 attacks = 0ULL;
    attacks = (((knight & (FILE_G | FILE_H)) ? 0ULL : (knight >> 6) | (knight << 10)) |
               ((knight & (FILE_A | FILE_B)) ? 0ULL : (knight >> 10) | (knight << 6)) |
               ((knight & FILE_H) ? 0ULL : (knight >> 15 | knight << 17)) |
               ((knight & FILE_A) ? 0ULL : (knight >> 17 | knight << 15)));

    return attacks; // Queried with U64 attacks = Knightattacks[from] & targets;
}

U64 ChessGame::GetBishopAttacks(U64 bishop, const U64 occupancy_) const
{
    return get_bishop_attacks(bishop, occupancy_);
}

U64 ChessGame::GetRookAttacks(U64 rook, const U64 occupancy_) const
{
    return get_rook_attacks(rook, occupancy_);
}

U64 ChessGame::GetQueenAttacks(U64 queen, const U64 occupancy_) const
{
    return get_bishop_attacks(queen, occupancy_) | get_rook_attacks(queen, occupancy_);
}

U64 ChessGame::GetKingAttacks(U64 king, const U64 occupancy_) const
{
    U64 attacks = 0ULL;
    attacks = (North(king) | NorthEast(king) | NorthWest(king) |
               South(king) | SouthEast(king) | SouthWest(king) |
               East(king) | West(king));

    return attacks;
}

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

U64 ChessGame::GetAttacks(Square square_, const U64 occupancy_) const
{
    U64 attacks, piece = 0ULL;
    set_bit(piece, square_);

    std::cout <<"PieceStart\n";
    PrintGoard();
    std::cout <<"PieceEnd\n";

    int which_function = -1;
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (piece & PieceTypeArray[i])
            which_function = i;
    }

    switch (which_function)
    {
    case 0:
        attacks = GetPawnAttacks(piece, board);
        break;
    case 1:
        attacks = GetKnightAttacks(piece, board);
        break;
    case 2:
        attacks = GetBishopAttacks(piece, board);
        break;
    case 3:
        attacks = GetRookAttacks(piece, board);
        break;
    case 4:
        attacks = GetQueenAttacks(piece, board);
        break;
    case 5:
        attacks = GetKingAttacks(piece, board);
        break;
    case -1:
        break;
    }

    return attacks;
}

U64 ChessGame::GetAttacks(Square square_) const
{
    return GetAttacks(square_, board);
}

// Checking functions
bool ChessGame::InCheck(Color color_of_king)
{

    U64 king = 0ULL;
    U64 attacking_piece = 0ULL;

    if (color_of_king == white)
    {
        king = WhitePiecesArray[King];
        Square king_square = static_cast<Square>(get_LSB(king));

        for (Piece piece_type = Pawn; piece_type <= King; piece_type = static_cast<Piece>(piece_type + 1))
        {

            clear_bit(WhitePiecesArray[King], king_square); //cleared king
            BlackPiecesArray[piece_type] |= (1ULL << king_square);
            /*
                        std::cout<<"PRINTIN PIECES START \n";
                        PrintGoard(BlackPiecesArray[piece_type]);
                        PrintGoard(BlackPiecesArray[King]);
                        std::cout<<"PRINTING PIECES ENDD n\n";
            */
            U64 attacks = GetAttacks(king_square, 0ULL); // Note: Pass 0ULL as occupancy since GetAttacks doesn't use it.

            clear_bit(BlackPiecesArray[piece_type], king_square); // cleared thing
            WhitePiecesArray[King] |= (1ULL << king_square);

            std::cout<<"startingprinting \n\n";
            PrintGoard(attacks);
            PrintGoard(attacking_piece);
            std::cout<<"ENDINGprinting \n\n";

            if (attacks & BlackPiecesArray[piece_type])
                return true;
        }
    }
    else
    {
        king = BlackPiecesArray[King];

        for (Piece piece_type = Pawn; piece_type <= King; piece_type = static_cast<Piece>(piece_type + 1))
        {
            Square king_square = static_cast<Square>(get_LSB(king));
            U64 attacks = GetAttacks(king_square, 0ULL); // Note: Pass 0ULL as occupancy since GetAttacks doesn't use it.
            attacking_piece = attacks & WhitePiecesArray[piece_type];

            PrintGoard(attacks);
            PrintGoard(attacking_piece);

            if (attacking_piece)
                return true;
        }
    }

    return false;
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
    set_bit(from, from_sq);

    if (!(from & board))
        return;

    Color from_color = GetColor(from);
    Piece its_piece = GetPieceType(from);

    if (from_color == white)
    {
        set_bit(WhitePiecesArray[its_piece], to_sq);
        clear_bit(WhitePiecesArray[its_piece], from_sq);
    }
    else
    {
        set_bit(BlackPiecesArray[its_piece], to_sq);
        clear_bit(BlackPiecesArray[its_piece], from_sq);
    }

    UpdateBoard();
}

