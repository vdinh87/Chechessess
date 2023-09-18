#pragma once
#include "SuperChessGame.hpp"
#include "AbilityLibrary.cpp"
SuperChessGame::SuperChessGame(const SuperPieceInfo &white)
{
    al = new AbilityLibrary(this);
    all_abilities.push_back(al->GetAbility(white));
}

SuperChessGame::~SuperChessGame()
{
    delete al;
    al = nullptr;
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

bool SuperChessGame::RemovePiece(Square square)
{
    U64 p = 0;
    set_bit(p, square);
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

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
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

    // before move
    if (from_color == white)
    {

        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
            Castle(from_sq, to_sq, GetCastling(from_color));
        else if (from_piece == Pawn && to_sq >= 56 && to_sq <= 63)
        {
            Promote(from_sq, to_sq, white, to_piece);
        }
        else if (EnPassant(from_sq, from_piece, from_color))
        {
            clear_bit(BlackPiecesArray[to_piece], prevMove.to);
            set_bit(WhitePiecesArray[from_piece], prevMove.to + 8);
            clear_bit(WhitePiecesArray[from_piece], from_sq);
        }
        else
        {
            clear_bit(BlackPiecesArray[to_piece], to_sq);
            set_bit(WhitePiecesArray[from_piece], to_sq);
            clear_bit(WhitePiecesArray[from_piece], from_sq);
        }
    }
    else if (from_color == black)
    {
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
            Castle(from_sq, to_sq, GetCastling(from_color));
        else if (from_piece == Pawn && to_sq >= 0 && to_sq <= 7)
            Promote(from_sq, to_sq, black, to_piece);
        else if (EnPassant(from_sq, from_piece, from_color))
        {
            clear_bit(WhitePiecesArray[to_piece], prevMove.to);
            set_bit(BlackPiecesArray[from_piece], prevMove.to - 8);
            clear_bit(BlackPiecesArray[from_piece], from_sq);
        }
        else
        {
            // on capture effect
            clear_bit(WhitePiecesArray[to_piece], to_sq);
            set_bit(BlackPiecesArray[from_piece], to_sq);
            clear_bit(BlackPiecesArray[from_piece], from_sq);
        }
    }
    // after move
    prevMove.from = from_sq;
    prevMove.to = to_sq;
    prevMove.is_pawn = (from_piece == Pawn);
    UpdateBoard();
    
}

void SuperChessGame::UseAbility()
{
    for (auto a : all_abilities)
    {
        a->Effect();
    }
}