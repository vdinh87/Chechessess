#include "RecursiveChessGame.hpp"
#include <memory>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iostream>

RecursiveChessGame::RecursiveChessGame() : ChessGame(), parent_game(nullptr), capture_from(invalid), capture_to(invalid), attacker_color(white)
{
}

RecursiveChessGame::RecursiveChessGame(RecursiveChessGame *parent, Square from, Square to, Color attacker)
    : ChessGame(), parent_game(parent), capture_from(from), capture_to(to), attacker_color(attacker)
{
    // Initialize a fresh board with starting positions
    WhitePiecesArray = {
        0x000000000000FF00ULL, // Pawns
        0x0000000000000042ULL, // Knights
        0x0000000000000024ULL, // Bishops
        0x0000000000000081ULL, // Rooks
        0x0000000000000008ULL, // Queen
        0x0000000000000010ULL  // King
    };

    BlackPiecesArray = {
        0x00FF000000000000ULL, // Pawns
        0x4200000000000000ULL, // Knights
        0x2400000000000000ULL, // Bishops
        0x8100000000000000ULL, // Rooks
        0x0800000000000000ULL, // Queen
        0x1000000000000000ULL  // King
    };

    InitMagics();
    UpdateBoard();
}

void RecursiveChessGame::SaveBoardState(std::vector<U64> &white_pieces, std::vector<U64> &black_pieces) const
{
    white_pieces = WhitePiecesArray;
    black_pieces = BlackPiecesArray;
}

void RecursiveChessGame::RestoreBoardState(const std::vector<U64> &white_pieces, const std::vector<U64> &black_pieces)
{
    WhitePiecesArray = white_pieces;
    BlackPiecesArray = black_pieces;
    UpdateBoard();
}

Color RecursiveChessGame::GetWinner() const
{
    if (IsWin(white))
        return white;
    if (IsWin(black))
        return black;
    return static_cast<Color>(-1); // No winner yet
}

std::vector<Action> RecursiveChessGame::Move(Square from_sq, Square to_sq)
{
    std::vector<Action> actions;
    U64 from = 0ULL;
    U64 to = 0ULL;
    set_bit(from, from_sq);
    set_bit(to, to_sq);

    // Basic validation
    U64 ally_pieces = GetColor(from) == white ? WhitePieces : BlackPieces;
    if (!(from & board) || (from_sq == to_sq) || (to & ally_pieces))
        return actions;

    Color from_color = GetColor(from);
    Piece from_piece = GetPieceType(from);
    Piece to_piece = GetPieceType(to);

    // Check if this move would be a capture
    bool is_capture = board & (1ULL << to_sq);

    // If this is a capture attempt and we're not in a sub-game, create a new sub-game
    if (is_capture && !IsSubGame())
    {
        // Save current board state
        std::vector<U64> saved_white_pieces, saved_black_pieces;
        SaveBoardState(saved_white_pieces, saved_black_pieces);

        // Create sub-game with current board state
        sub_game = std::make_unique<RecursiveChessGame>(this, from_sq, to_sq, from_color);

        // Store the saved state for later restoration
        saved_white_pieces = WhitePiecesArray;
        saved_black_pieces = BlackPiecesArray;

        // Wait for sub-game to complete
        // TODO: This should be handled by the UI/game driver
        // For now, we'll just return a special action to indicate a sub-game should start
        actions.push_back(Action::Abilityes); // Using Abilityes as our signal for sub-game
        return actions;
    }

    // Normal move execution
    if (from_color == white)
    {
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {
            // Castling logic
            U64 valid_moves = GetCastling(from_color);
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            if (to_sq == c1 && get_bit(valid_moves, c1))
            {
                ExecuteMove(from_color, a1, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, King);
            }
            else if (to_sq == g1 && get_bit(valid_moves, g1))
            {
                ExecuteMove(from_color, h1, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);
            }
            actions.push_back(Action::Castle);
        }
        else if (from_piece == Pawn && to_sq >= 56 && to_sq <= 63)
        {
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        }
        else if (EnPassant(from_sq, from_piece, from_color))
        {
            ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) + 8), from_piece, to_piece);
            RemovePiece(GetPreviousMove().to);
            actions.push_back(Action::Capture);
        }
        else
        {
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            if (is_capture)
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }
    else
    {
        // Similar logic for black pieces
        if ((from_piece == King) && (GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {
            U64 valid_moves = GetCastling(from_color);
            ExecuteMove(from_color, from_sq, to_sq, King, King);
            if (to_sq == c8 && get_bit(valid_moves, c8))
                ExecuteMove(from_color, a8, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, Rook);
            else if (to_sq == g8 && get_bit(valid_moves, g8))
                ExecuteMove(from_color, h8, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);
            actions.push_back(Action::Castle);
        }
        else if (from_piece == Pawn && to_sq >= 0 && to_sq <= 7)
        {
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        }
        else if (EnPassant(from_sq, from_piece, from_color))
        {
            ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) - 8), from_piece, to_piece);
            RemovePiece(GetPreviousMove().to);
            actions.push_back(Action::Capture);
        }
        else
        {
            ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            if (is_capture)
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }

    // Check for check and checkmate
    Color opponent_color = static_cast<Color>(!static_cast<bool>(from_color));
    if (InCheck(board, opponent_color, 0))
        actions.push_back(Check);

    UpdateBoard();

    // Only check for win if this is a sub-game or if there's no capture to resolve
    if (IsSubGame() || !(board & (1ULL << to_sq)))
    {
        if (IsWin(white) || IsWin(black))
        {
            actions.push_back(Checkmate);

            // If this is a sub-game, apply the capture result to the parent game
            if (IsSubGame() && parent_game)
            {
                Color winner = IsWin(white) ? white : black;
                if (winner == attacker_color)
                {
                    // Attacker wins - execute the capture in parent game
                    parent_game->RemovePiece(capture_to); // Remove defending piece first
                    parent_game->ExecuteMove(attacker_color, capture_from, capture_to,
                                             parent_game->GetPieceType(1ULL << capture_from),
                                             parent_game->GetPieceType(1ULL << capture_to));
                    parent_game->UpdateBoard();
                }
                else
                {
                    // Defender wins - remove the attacking piece
                    parent_game->RemovePiece(capture_from);
                    parent_game->UpdateBoard();
                }
            }
        }
    }

    return actions;
}

void RecursiveChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    // In a sub-game, execute moves normally
    if (IsSubGame())
    {
        ChessGame::ExecuteMove(color, from_sq, to_sq, from_piece, to_piece);
        UpdateBoard();
        return;
    }

    // In the main game, captures are handled through sub-games
    if (board & (1ULL << to_sq))
    {
        // Don't execute the capture yet - it will be handled after the sub-game
        return;
    }

    // Non-capture moves execute normally
    ChessGame::ExecuteMove(color, from_sq, to_sq, from_piece, to_piece);
    UpdateBoard();
}