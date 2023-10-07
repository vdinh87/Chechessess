#pragma once
#include "SuperChessGame.hpp"
#include "AbilityLibrary.cpp"
#include "SuperPiece.cpp"
SuperChessGame::SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // InitSuperPieces(white, black);
    // all_abilities.push_back(al->GetAbility(black));
    al = std::make_shared<AbilityLibrary>(this);
    std::vector<std::unique_ptr<Ability>> v;
    v.push_back(al->GetAbility(white_info));
    super_pieces[Square::a1] = std::make_shared<SuperPiece>(v, white_info, Square::a1, Color::white);
}

bool SuperChessGame::AddPiece(Square square, Color color, Piece piece)
{
    U64 p = 0ULL;
    set_bit(p, square);
    if (!(board & p) && color == white)
        return WhitePiecesArray[piece] |= p;
    else if (!(board & p) && color == black)
        return BlackPiecesArray[piece] |= p;

    UpdateBoard();
    return false;
}

bool SuperChessGame::AddSuperPiece(SuperPieceInfo info, Square square, Color color)
{
    return false;
}

bool SuperChessGame::AddSuperPiecesofType(SuperPieceInfo info, Color color)
{
    return false;
}

bool SuperChessGame::RemovePiece(Square square)
{
    U64 p = 0;
    set_bit(p, square);
    // no piece on board
    if (!(p & board))
        return false;
    if (((1ULL << square) & PieceTypeArray[King]) != 0)
    {
        std::cout << "You may not remove the King from the Board!!\n";
        return false;
    }

    Color color = GetColor(p);
    if (color == white)
        clear_bit(WhitePiecesArray[GetPieceType(p)], square);
    else
        clear_bit(BlackPiecesArray[GetPieceType(p)], square);

    UpdateBoard();
    return true;
}

bool AddSuperPiecesofType(SuperPieceInfo info, Color color)
{
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
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
            executeMove(from_color, from_sq, to_sq, from_piece, to_piece);
            std::cout << "Test\n";
            // Checks which way
            if (to_sq == c1 && get_bit(valid_moves, c1))
            {
                executeMove(from_color, a1, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, King);
            }
            else if (to_sq == g1 && get_bit(valid_moves, g1))
                executeMove(from_color, h1, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        } // Promotion conditions
        else if (from_piece == Pawn && to_sq >= 56 && to_sq <= 63)
        { // Promotion
            Piece promoting_to_piece = promoteInput(from_sq, to_sq, from_color, to_piece);
            executeMove(from_color, from_sq, to_sq, promoting_to_piece, to_piece);
            actions.push_back(Action::Promotion);
        } // En passant conditions
        else if (EnPassant(from_sq, from_piece, from_color))
        { // En passant
            executeMove(from_color, from_sq, static_cast<Square>(static_cast<int>(prevMove.to) + 8), from_piece, to_piece);
            clear_bit(BlackPiecesArray[to_piece], prevMove.to);
            actions.push_back(Capture);
        }
        else // Normal Move
        {
            executeMove(from_color, from_sq, to_sq, from_piece, to_piece);
            actions.push_back(Action::Move);
        }
    }
    else if (from_color == black)
    { // Castling Conditions
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {                                              // Castling
            U64 valid_moves = GetCastling(from_color); // does null check and InCheck
            executeMove(from_color, from_sq, to_sq, King, King);
            // Checks which way
            if (to_sq == c8 && get_bit(valid_moves, c8))
                executeMove(from_color, a8, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, Rook);
            else if (to_sq == g8 && get_bit(valid_moves, g8))
                executeMove(from_color, h8, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        }
        else if (from_piece == Pawn && to_sq >= 0 && to_sq <= 7)
        { // Promotion
            Piece promoting_to_piece = promoteInput(from_sq, to_sq, from_color, to_piece);
            executeMove(from_color, from_sq, to_sq, promoting_to_piece, to_piece);
            actions.push_back(Action::Promotion);
        } // Enpassant conditoins
        else if (EnPassant(from_sq, from_piece, from_color))
        { // Enpassant
            executeMove(from_color, from_sq, static_cast<Square>(static_cast<int>(prevMove.to) - 8), from_piece, to_piece);
            clear_bit(WhitePiecesArray[to_piece], prevMove.to);
            actions.push_back(Capture);
        } // Normal move
        else
        {
            executeMove(from_color, from_sq, to_sq, from_piece, to_piece);
            actions.push_back(Action::Move);
        }
    }

    UpdatePrevMove(from_sq, to_sq, from_piece);
    UpdateBoard();
    return actions;
}

//init
void SuperChessGame::InitSuperPieces(const SuperPieceInfo &white, const SuperPieceInfo &black)
{
    
}

//misc
void SuperChessGame::Do(Square sq)
{
    super_pieces[sq]->UseAbility();
}
