#pragma once
#include "Magics.cpp"

class ChessGame
{
private:
    // board
    std::vector<U64> WhitePiecesArray;
    std::vector<U64> BlackPiecesArray;
    std::vector<U64> PieceTypeArray;
    U64 WhitePieces;
    U64 BlackPieces;
    U64 board;

    //for castling
    ChessMove prevMove;

    // Init magics
    void InitMagics() const;
    U64 Mask(Piece piece, Color color, const U64 &other_board) const;

    // get attacks
    U64 GetKnightAttacks(Square square, const U64 occupancy_) const;
    U64 GetBishopAttacks(Square square, const U64 occupancy_) const;
    U64 GetRookAttacks(Square square, const U64 occupancy_) const;
    U64 GetQueenAttacks(Square square, const U64 occupancy_) const;
    U64 GetKingAttacks(Square square, const U64 occupancy_) const;
    U64 GetPawnAttacks(Square square, const U64 occupancy_) const;
    U64 GetAttacks(Square square_, const U64 occupancy_, int which_function = -1) const;

    // checking
    U64 InCheck(const U64 &occupancy_, Color color_of_king, int offset) const;
    U64 InCheck(const U64 &occupancy_, Color color_of_king, const U64 &king) const;

    // filter functions
    U64 FilterTeam(const U64 &moveset, const U64 &piece) const;
    U64 FilterCheck(const U64 &moveset, const U64 &piece) const;
    U64 FilterPin(const U64 &moveset, const U64 &piece) const;
    U64 FilterLegalKingMoves(const U64 &moveset, const U64 &piece) const;

    // Utility functions
    Color GetColor(U64 piece) const;
    Piece GetPieceType(U64 unknown_piece) const;
    bool IsSlider(const U64 board_) const;
    void UpdateBoard();

    // Special move functions
    U64 GetCastling(Color color) const;
    void Castle(Square from_sq, Square to_sq, U64 valid_moves);
    U64 GetEnPassant(Square square, const U64 occupancy_, Color color) const;
    bool EnPassant(Square square, Piece type, Color color) const;
    void Promote(Square from_sq, Square to_sq, Color color, Piece to_piece);

public:
    ChessGame();
    ~ChessGame() = default;
    U64 GetAttacks(Square square_) const;
    void Move(Square from_sq, Square to_sq);
    void PrintBoard() const;
};