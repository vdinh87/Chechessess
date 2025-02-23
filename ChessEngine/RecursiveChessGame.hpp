#pragma once
#include "ChessGame.hpp"
#include <memory>
#include <vector>

class RecursiveChessGame : public ChessGame
{
protected:
    // Store the parent game and capture information when this is a sub-game
    RecursiveChessGame *parent_game;
    Square capture_from;
    Square capture_to;
    Color attacker_color;

    // Store the current sub-game if one is in progress
    std::unique_ptr<RecursiveChessGame> sub_game;

    // Override move execution to handle captures differently
    void ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece) override;

    // Helper to save current board state
    void SaveBoardState(std::vector<U64> &white_pieces, std::vector<U64> &black_pieces) const;

    // Helper to restore board state
    void RestoreBoardState(const std::vector<U64> &white_pieces, const std::vector<U64> &black_pieces);

public:
    RecursiveChessGame();

    // Constructor for sub-games
    RecursiveChessGame(RecursiveChessGame *parent, Square from, Square to, Color attacker);

    // Override move to handle capture attempts
    std::vector<Action> Move(Square from_sq, Square to_sq) override;

    // Check if this is a sub-game
    bool IsSubGame() const { return parent_game != nullptr; }

    // Get the winner's color when game ends
    Color GetWinner() const;
};