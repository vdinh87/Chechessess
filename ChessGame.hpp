#pragma once
#include "Magics.cpp"

class ChessGame
{
private:
    //board
    std::vector<U64> WhitePiecesArray;
    std::vector<U64> BlackPiecesArray;
    std::vector<U64> PieceTypeArray;
    U64 WhitePieces;
    U64 BlackPieces;
    U64 board;

    //Innit magics
    void InitMagics() const;
    U64 Mask (Piece piece, Color color, const U64& other_board) const;

    //get attacks
    U64 GetPawnAttacks   (U64 position_, const U64 occupancy_) const;
    U64 GetKnightAttacks (U64 position_, const U64 occupancy_) const;
    U64 GetBishopAttacks (U64 position_, const U64 occupancy_) const;
    U64 GetRookAttacks   (U64 position_, const U64 occupancy_) const;
    U64 GetQueenAttacks  (U64 position_, const U64 occupancy_) const;
    U64 GetKingAttacks   (U64 position_, const U64 occupancy_) const;

    //filter functions
    U64 FilterAllMoves(const U64& otherBoard, Color color) const;
    U64 FilterCheck(const U64& otherBoard, Color color) const;
    U64 FilterTeam(const U64& otherBoard, Color color) const;

    //Utility functions
    Color GetColor(U64 piece) const;
    Piece GetPieceType(U64 unknown_piece) const;
    void UpdateBoard();
public:
    ChessGame();
    ~ChessGame() = default;
    bool InCheck(Color color_of_king);
    U64 GetAttacks(Square square_, const U64 occupancy_) const;
    U64 GetAttacks(Square square_) const;
    void Move(Square from_sq, Square to_sq);
    void PrintBoard() const;
};